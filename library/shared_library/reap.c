/*
 * reap.c - a program closed from outside, and what it leaves in this library.
 *
 * A program closed by the kernel never reaches libClose. Its threads would run on, on a
 * context nobody will end; the parent waiting for it would wait for ever; the files
 * and scans the library opened for it would stay open, since dos does not record what
 * a library opens. This client stops the threads, tells the parent, and declares those
 * files to the kernel's ledger as they are opened, so dos closes them with the rest.
 * A program is told apart by its task and task ID, kept here from the moment it is
 * fully open: pr_UID is copied to every child, and says nothing by itself.
 *
 * The kernel's reaper tells this client and waits for nobody, so a task of its own does
 * the work. A lock it needs may be held by anyone. The kernel lets go of a closed task's
 * locks before the first round, so one a forced kill was inside guards something left half
 * done: nothing here waits long for a lock, and once any task of a program was closed
 * that way nothing here takes one for that program, or walks the tables they guard. The
 * context is read only once nobody of the program runs.
 * The heap, the context and the program's mappings stay: nothing here can know that
 * nobody reaches them. The last kill of a program's tasks tells the kernel's report which
 * blocks its heap is made of, and what speaks against judging them; the report judges,
 * and nothing is freed. What the rounds could not do stays too, and is said.
 *
 * Sockets are bsdsocket's to close: from here they would close on the wrong context.
 *
 * Written for AROS4. No part of this file is derived from any AmigaOS implementation.
 */
#undef __USE_INLINE__
#define __NOLIBBASE__
#define __NOGLOBALIFACE__

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/timer.h>
#include <exec/memory.h>
#include <exec/tasks.h>
#include <dos/dosextens.h>
#include <execext/execext.h>

#include <signal.h>
#include <string.h>

#include "clib4.h"
#include "map.h"
#include "dos.h"
#include "reap.h"
#include "children.h"
#include "../posix/ipc_headers.h"
#include "../socket/socket_headers.h"
#include "../wmem/wmem_allocator_block.h"

extern struct ExecIFace *IExec;
extern struct DOSIFace *IDOS;

/* The toolchain's headers may not have this one. */
#ifndef TASKATTR_TASKID
#define TASKATTR_TASKID (TAG_USER + 0x630)
#endif

#define REAP_PROGRAMS       256
#define REAP_PRIORITY       10
#define REAP_STACK          16384

/* Live records for one program: above this the ledger is left for the others. */
#define REAP_TRACK_MAX      128

/* Tries of 3 ticks each, under a second, so that two locks stay well under what the reaper waits for a client. */
#define REAP_LOCK_TRIES     16
#define REAP_LOCK_TICKS     3

/* How long a member that said no is asked again: a reply by 2 s, and on alone until 5 s. */
#define REAP_REPLY_SECS     2
#define REAP_RETRY_SECS     5
#define REAP_RETRY_TICKS    5

/* How many tasks a look at the system holds. With more than this it cannot tell who runs. */
#define REAP_LIST_ROWS      512

#define REAP_THREAD_NAME    "pthread id #"
#define REAP_TIMER_NAME     "ITIMER_TASK"

/* Members closed by a helper, by task ID, whose kill is counted before the reaper starts on it. */
#define REAP_KILLED         16

/* What a kill of one member says that holds for the whole program's heap. */
#define REAP_GROUP_FLAGS    (RRF_STALLED | RRF_REBOOTADVISED | RRF_REQUESTSLEFT | RRF_PACKETOUT)

/* What a parent reads from a child that was killed: the shell's form of a SIGKILL. */
#define REAP_KILLED_STATUS  (128 + SIGKILL)

/*
 * One program. Read with no lock, by the reaper too, which must never wait: the words
 * are claimed by rp_Ctx and published by rp_Task, written last and cleared first.
 * The task stays here after it was closed, for as long as members of it run or wait to be
 * reaped: the kill of each member is counted from its first round to its last, and the
 * one that brings the count to nothing speaks for the whole program's heap.
 */
struct ReapProgram {
    struct Task *volatile   rp_Task;
    ULONG                   rp_TaskID;
    struct _clib4 *volatile rp_Ctx;
    volatile LONG           rp_Stopping;    /* helpers still asking members to stop */
    volatile LONG           rp_Finished;    /* one helper does the last work        */
    volatile LONG           rp_Jobs;        /* kills of members not at their last round */
    volatile ULONG          rp_Flags;       /* REAP_GROUP_FLAGS of all of them so far */
    volatile ULONG          rp_Killed[REAP_KILLED];
};

static struct ReapProgram reapPrograms[REAP_PROGRAMS];
static struct ReapClient reapClient;
static const char reapName[] = "clib4 reap";

/* Helpers that have not ended: the library's code stays until they have. */
static volatile LONG reapHelpers;

static ULONG reap_task_id(struct Task *task)
{
    ULONG id = 0;
    struct TagItem tags[] = {
        { TASKATTR_TASKID, (ULONG)&id },
        { TAG_DONE,        0 }
    };

    IExec->GetTaskAttrs(task, tags);
    return id;
}

/* -------------------------------------------------------------------------- */
/* Programs coming and going                                                   */
/* -------------------------------------------------------------------------- */

static void program_clear(struct ReapProgram *rp)
{
    rp->rp_Task = NULL;
    __sync_synchronize();
    rp->rp_Ctx = NULL;
}

void __reap_program_add(struct _clib4 *__clib4)
{
    struct Task *me = IExec->FindTask(NULL);
    ULONG id = reap_task_id(me);
    int i;

    /* Anything under this task or context now is an earlier one that went without saying. */
    for (i = 0; i < REAP_PROGRAMS; i++) {
        struct ReapProgram *rp = &reapPrograms[i];

        if (rp->rp_Task == me || rp->rp_Ctx == __clib4)
            program_clear(rp);
    }
    for (i = 0; i < REAP_PROGRAMS; i++) {
        struct ReapProgram *rp = &reapPrograms[i];

        if (__sync_bool_compare_and_swap(&rp->rp_Ctx, NULL, __clib4)) {
            int k;

            rp->rp_TaskID = id;
            rp->rp_Finished = 0;
            rp->rp_Jobs = 0;
            rp->rp_Flags = 0;
            for (k = 0; k < REAP_KILLED; k++)
                rp->rp_Killed[k] = 0;
            __sync_synchronize();
            rp->rp_Task = me;
            return;
        }
    }
    /* Not known here: closed from outside it is only what dos knows of it. */
}

void __reap_program_remove(struct _clib4 *__clib4)
{
    int i;

    for (i = 0; i < REAP_PROGRAMS; i++)
        if (reapPrograms[i].rp_Ctx == __clib4)
            program_clear(&reapPrograms[i]);
}

/* -------------------------------------------------------------------------- */
/* The ledger                                                                  */
/* -------------------------------------------------------------------------- */

static BOOL track(struct _clib4 *__clib4, ULONG tag, APTR obj)
{
    if (!ExecExtAvailable(IExec) || __clib4->__reap_tracked >= REAP_TRACK_MAX)
        return FALSE;
    if (!ExecExt(IExec)->TrackAdd(IExec, NULL, tag, obj))
        return FALSE;
    __sync_fetch_and_add(&__clib4->__reap_tracked, 1);
    return TRUE;
}

BOOL __reap_track_file(struct _clib4 *__clib4, BPTR file)
{
    return track(__clib4, DOS_TRACK_FILE, (APTR)file);
}

BOOL __reap_track_lock(struct _clib4 *__clib4, BPTR lock)
{
    return track(__clib4, DOS_TRACK_LOCK, (APTR)lock);
}

BOOL __reap_track_dir(struct _clib4 *__clib4, APTR context)
{
    return track(__clib4, DOS_TRACK_DIR, context);
}

/* dos forgets the record when it closes the object: this only keeps the count. */
void __reap_untracked(struct _clib4 *__clib4)
{
    __sync_fetch_and_sub(&__clib4->__reap_tracked, 1);
}

/* -------------------------------------------------------------------------- */
/* Who belongs to a program                                                    */
/* -------------------------------------------------------------------------- */

/* A thread of the program with that context; only compared with it. */
static BOOL is_thread_of(struct Task *task, struct _clib4 *ctx)
{
    struct Process *proc = (struct Process *)task;

    return task->tc_Node.ln_Type == NT_PROCESS && (struct _clib4 *)proc->pr_UID == ctx &&
           (proc->pr_Flags & PRF_CHILDPROCESS) != 0;
}

static int program_of(struct Task *victim)
{
    ULONG id = 0;
    BOOL  gotID = FALSE;
    int   i;

    for (i = 0; i < REAP_PROGRAMS; i++) {
        struct ReapProgram *rp = &reapPrograms[i];

        if (rp->rp_Task == victim) {
            if (!gotID) {
                id = reap_task_id(victim);
                gotID = TRUE;
            }
            if (rp->rp_TaskID == 0 || id == 0 || rp->rp_TaskID == id)
                return i;
        }
    }

    if (victim->tc_Node.ln_Type != NT_PROCESS || victim->tc_Node.ln_Name == NULL)
        return -1;
    if (strncmp((const char *)victim->tc_Node.ln_Name, REAP_THREAD_NAME, sizeof(REAP_THREAD_NAME) - 1) != 0)
        return -1;
    for (i = 0; i < REAP_PROGRAMS; i++) {
        struct ReapProgram *rp = &reapPrograms[i];
        struct _clib4 *ctx = rp->rp_Ctx;

        if (ctx != NULL && rp->rp_Task != NULL && is_thread_of(victim, ctx))
            return i;
    }
    return -1;
}

/*
 * Who of the program is running now, in rows, with the rows of the others emptied, and
 * its timers told to end if asked. A task stopped for closing is on no list, so a member seen here is
 * one that still runs; a task may end straight after this, so a row holds only what was
 * read while nothing could. TRUE if the rows are all the tasks there are.
 */
static BOOL look_at_members(struct ReapProgram *rp, struct _clib4 *ctx, struct Task *victim,
                            struct ExecTaskInfo *rows, ULONG *count, BOOL endTimers)
{
    struct ExecTaskListInfo info;
    struct Task *mainTask = rp->rp_Task;
    ULONG mainID = rp->rp_TaskID, i, n, kept = 0;

    IExec->Forbid();
    n = ExecExt(IExec)->GetTaskList(IExec, rows, REAP_LIST_ROWS, &info);
    for (i = 0; i < n; i++) {
        struct Task *task = rows[i].eti_Task;
        BOOL member = FALSE;

        if (task == victim)
            continue;
        if (task == mainTask && (mainID == 0 || rows[i].eti_TaskID == 0 || rows[i].eti_TaskID == mainID))
            member = TRUE;
        else if (rows[i].eti_Type == NT_PROCESS) {
            if (strncmp(rows[i].eti_Name, REAP_THREAD_NAME, sizeof(REAP_THREAD_NAME) - 1) == 0)
                member = is_thread_of(task, ctx);
            else if (endTimers && strcmp(rows[i].eti_Name, REAP_TIMER_NAME) == 0 &&
                     (struct _clib4 *)((struct Process *)task)->pr_UID == ctx)
                IExec->Signal(task, SIGBREAKF_CTRL_F);
        }
        if (member)
            rows[kept++] = rows[i];
    }
    IExec->Permit();
    *count = kept;
    return info.etl_Total <= REAP_LIST_ROWS;
}

/*
 * Whether anything of the program still runs: its main task, its threads by pr_UID whatever
 * their name, and a member another helper is asking to stop, which may be stopped for a
 * moment and on no list. With anyone, any process with its pr_UID counts too, as a child
 * that is no thread, a console's handler among them, has it. What cannot be looked at
 * counts as running. A wrong yes only keeps things.
 */
static BOOL others_run(struct ReapProgram *rp, struct _clib4 *ctx, struct Task *victim, BOOL anyone)
{
    struct ExecTaskInfo *rows;
    struct ExecTaskListInfo info;
    BOOL run = FALSE;
    ULONG i, n;

    rows = IExec->AllocVecTags(REAP_LIST_ROWS * sizeof(*rows), AVT_Type, MEMF_SHARED, TAG_DONE);
    if (rows == NULL)
        return TRUE;
    IExec->Forbid();
    n = ExecExt(IExec)->GetTaskList(IExec, rows, REAP_LIST_ROWS, &info);
    for (i = 0; i < n && !run; i++) {
        struct Task *task = rows[i].eti_Task;

        if (task == victim)
            continue;
        if ((task == rp->rp_Task && (rp->rp_TaskID == 0 || rows[i].eti_TaskID == rp->rp_TaskID)) ||
            (rows[i].eti_Type == NT_PROCESS &&
             (anyone ? (struct _clib4 *)((struct Process *)task)->pr_UID == ctx : is_thread_of(task, ctx))))
            run = TRUE;
    }
    IExec->Permit();
    IExec->FreeVec(rows);
    if (info.etl_Total > REAP_LIST_ROWS)
        run = TRUE;
    /* Read after the list: a member stopped by a helper is counted, or the helper is still at it. */
    __sync_synchronize();
    if (rp->rp_Stopping != 0)
        run = TRUE;
    return run;
}

/* -------------------------------------------------------------------------- */
/* The rounds                                                                  */
/* -------------------------------------------------------------------------- */

static ULONG seconds_now(void)
{
    extern struct TimerIFace *ITimer;
    struct TimeVal tv;

    ITimer->GetSysTime(&tv);
    return tv.Seconds;
}

/*
 * A member about to be closed by a helper: its kill is counted now, as the reaper may start on
 * it before KillTaskID has returned here. The slot, or -1 when none was free: then its first
 * round counts it a second time, and the program's heap is never spoken for.
 */
static int killed_add(struct ReapProgram *rp, ULONG id)
{
    int k;

    __sync_fetch_and_add(&rp->rp_Jobs, 1);
    for (k = 0; id != 0 && k < REAP_KILLED; k++)
        if (__sync_bool_compare_and_swap(&rp->rp_Killed[k], 0, id))
            return k;
    return -1;
}

/* The kill did not happen, so no round of it will come. */
static void killed_undo(struct ReapProgram *rp, int slot)
{
    if (slot >= 0)
        rp->rp_Killed[slot] = 0;
    __sync_fetch_and_sub(&rp->rp_Jobs, 1);
}

/*
 * In the reaper, at a member's first round: counted, unless a helper counted it when it closed it.
 * A forced kill says so for the whole program from here, before any member's later round.
 */
static void count_kill(struct ReapProgram *rp, struct ReapMsg *msg)
{
    struct Task *victim = msg->rm_Task;
    ULONG id = reap_task_id(victim);
    int k;

    if (msg->rm_Flags & RRF_REBOOTADVISED)
        __sync_fetch_and_or(&rp->rp_Flags, RRF_REBOOTADVISED);

    for (k = 0; id != 0 && k < REAP_KILLED; k++)
        if (rp->rp_Killed[k] == id && __sync_bool_compare_and_swap(&rp->rp_Killed[k], id, 0))
            return;
    __sync_fetch_and_add(&rp->rp_Jobs, 1);
}

/*
 * Its timers stop raising signals in its name, and every other member is closed too: they
 * would run on what was left half done. A member that holds a lock in a library is asked
 * again for a while; the round is answered after 2 s at most and this goes on alone.
 */
static void stop_members(struct ReapMsg *msg, struct ReapProgram *rp)
{
    struct Task *victim = msg->rm_Task;
    struct _clib4 *ctx = rp->rp_Ctx;
    char name[32];
    struct ExecTaskInfo *rows;
    ULONG start, left, i;
    BOOL replied = FALSE, firstLook = TRUE;

    strncpy(name, victim->tc_Node.ln_Name != NULL ? (const char *)victim->tc_Node.ln_Name : "?", sizeof(name) - 1);
    name[sizeof(name) - 1] = '\0';

    if (ctx == NULL) {
        IExec->ReplyMsg(&msg->rm_Msg);
        return;
    }
    rows = IExec->AllocVecTags(REAP_LIST_ROWS * sizeof(*rows), AVT_Type, MEMF_SHARED, TAG_DONE);
    if (rows == NULL) {
        IExec->DebugPrintF("clib4: '%s': no memory to look for its threads\n", name);
        IExec->ReplyMsg(&msg->rm_Msg);
        return;
    }

    /* One asks for all of them: this member's helper finds the one asking already. */
    if (__sync_fetch_and_add(&rp->rp_Stopping, 1) != 0) {
        __sync_fetch_and_sub(&rp->rp_Stopping, 1);
        IExec->FreeVec(rows);
        IExec->ReplyMsg(&msg->rm_Msg);
        return;
    }

    /* The word is the timer's own stop, and a program that started leaving is not read. */
    IExec->Forbid();
    if (rp->rp_Ctx == ctx && rp->rp_Task != NULL)
        ctx->__signals_blocked |= sigmask(SIGALRM);
    IExec->Permit();

    start = seconds_now();
    for (;;) {
        ULONG count;

        look_at_members(rp, ctx, victim, rows, &count, firstLook);
        firstLook = FALSE;
        left = 0;
        for (i = 0; i < count; i++) {
            int slot = killed_add(rp, rows[i].eti_TaskID);
            LONG res = ExecExt(IExec)->KillTaskID(IExec, rows[i].eti_Task, rows[i].eti_TaskID, KTF_ONCE, NULL);

            if (res > 0)
                continue;
            killed_undo(rp, slot);
            if (res == KTE_NOTASK || res == KTE_EXITING)
                continue;
            if (res == KTE_DIRTY || res == KTE_BUSY || res == KTE_CANNOTSTOP)
                left++;
            else
                IExec->DebugPrintF("clib4: '%s': '%s' of its program stays running, the answer was %ld\n", name, rows[i].eti_Name, res);
        }
        if (left == 0)
            break;
        if (seconds_now() - start >= REAP_RETRY_SECS) {
            IExec->DebugPrintF("clib4: '%s': %lu members of its program stay running, each holds a lock\n", name, left);
            break;
        }
        if (!replied && seconds_now() - start >= REAP_REPLY_SECS) {
            replied = TRUE;
            IExec->ReplyMsg(&msg->rm_Msg);
        }
        IDOS->Delay(REAP_RETRY_TICKS);
    }

    __sync_fetch_and_sub(&rp->rp_Stopping, 1);
    IExec->FreeVec(rows);
    if (!replied)
        IExec->ReplyMsg(&msg->rm_Msg);
}

/* Under a second, and it may come to nothing. */
static BOOL try_semaphore(struct SignalSemaphore *sem)
{
    int tries;

    for (tries = 0; tries < REAP_LOCK_TRIES; tries++) {
        if (IExec->AttemptSemaphore(sem))
            return TRUE;
        IDOS->Delay(REAP_LOCK_TICKS);
    }
    return FALSE;
}

/* A parent waiting for this child goes on. By its process ID: nothing here holds an entry. */
static ULONG tell_parent(struct Clib4Resource *res, struct Process *victim)
{
    size_t iter = 0;
    void *item;
    ULONG told = 0;

    while (hashmap_iter(res->children, &iter, &item)) {
        const struct Clib4Node *node = item;

        if (node->spawnedProcesses != NULL) {
            struct Clib4Children key;
            struct Clib4Children *child;

            memset(&key, 0, sizeof(key));
            key.pid = victim->pr_ProcessID;
            child = (struct Clib4Children *)hashmap_get(node->spawnedProcesses, &key);
            if (child != NULL) {
                child->returnCode = REAP_KILLED_STATUS;
                told = 1;
                break;
            }
        }
    }
    return told;
}

/* The names it bound, nobody can connect to those now. */
static ULONG forget_unix_names(struct Clib4Resource *res, struct _clib4 *ctx)
{
    size_t iter = 0;
    void *item;
    ULONG count = 0;

    while (hashmap_iter(res->uxSocketsMap, &iter, &item)) {
        const struct UnixSocket *socket = item;

        if (socket->owner == ctx) {
            hashmap_delete(res->uxSocketsMap, (void *)socket);
            iter = 0;
            count++;
        }
    }
    return count;
}

/*
 * Its entry goes unless a child of it may still come to read what was left for it.
 * Its undo records are in its heap and stay with that.
 */
static ULONG forget_node(struct Clib4Resource *res, struct _clib4 *ctx)
{
    size_t iter = 0;
    void *item;

    while (hashmap_iter(res->children, &iter, &item)) {
        const struct Clib4Node *node = item;

        if (node->pid == (uint32)ctx->processId) {
            if (node->spawnedProcesses != NULL) {
                size_t childIter = 0;
                void *childItem;

                while (hashmap_iter(node->spawnedProcesses, &childIter, &childItem))
                    if (((struct Clib4Children *)childItem)->fdInherit != NULL)
                        return 0;
                hashmap_free(node->spawnedProcesses);
            }
            hashmap_delete(res->children, (void *)node);
            return 1;
        }
    }
    return 0;
}

/* Nobody of the program runs: what it held in tables others read goes, once. */
static ULONG finish_program(struct ReapMsg *msg, struct Clib4Resource *res, struct ReapProgram *rp, struct _clib4 *ctx)
{
    const char *name = msg->rm_Task->tc_Node.ln_Name != NULL ? (const char *)msg->rm_Task->tc_Node.ln_Name : "?";
    ULONG count = 0;

    if (!__sync_bool_compare_and_swap(&rp->rp_Finished, 0, 1))
        return 0;

    if (msg->rm_Flags & RRF_REBOOTADVISED) {
        IExec->DebugPrintF("clib4: '%s': it held a lock, its tables stay\n", name);
    } else if (rp->rp_Flags & RRF_REBOOTADVISED) {
        IExec->DebugPrintF("clib4: '%s': a task of its program held a lock, its tables stay\n", name);
    } else {
        if (ctx->__shm_tracking_count > 0) {
            if (try_semaphore(res->shmcx.keymap.Lock)) {
                __shm_detach_all(res, ctx);
                IExec->ReleaseSemaphore(res->shmcx.keymap.Lock);
                count++;
            } else
                IExec->DebugPrintF("clib4: '%s': its shared memory stays attached\n", name);
        }
        if (try_semaphore(&res->mapLock)) {
            count += forget_unix_names(res, ctx);
            count += forget_node(res, ctx);
            IExec->ReleaseSemaphore(&res->mapLock);
        } else
            IExec->DebugPrintF("clib4: '%s': its entries in the tables stay\n", name);
    }
    return count;
}

/* A member still runs and may use them: they stay open, and dos is not told. */
static ULONG keep_records(struct Task *victim)
{
    static const ULONG tags[] = { DOS_TRACK_FILE, DOS_TRACK_DIR, DOS_TRACK_LOCK };
    ULONG count = 0, i;
    APTR obj;

    for (i = 0; i < sizeof(tags) / sizeof(tags[0]); i++)
        while (ExecExt(IExec)->TrackTake(IExec, victim, tags[i], &obj))
            count++;
    return count;
}

static void libc_round(struct ReapMsg *msg, int program)
{
    struct Task *victim = msg->rm_Task;
    const char *name = victim->tc_Node.ln_Name != NULL ? (const char *)victim->tc_Node.ln_Name : "?";
    struct Clib4Resource *res = (APTR)IExec->OpenResource(RESOURCE_NAME);
    BOOL dirty = (msg->rm_Flags & RRF_REBOOTADVISED) != 0 ||
                 (program >= 0 && (reapPrograms[program].rp_Flags & RRF_REBOOTADVISED) != 0);

    if (res != NULL && victim->tc_Node.ln_Type == NT_PROCESS &&
        ((struct Process *)victim)->pr_ExitCode == (APTR)spawnedProcessExit) {
        if (!dirty && try_semaphore(&res->mapLock)) {
            msg->rm_LibcObjects += tell_parent(res, (struct Process *)victim);
            IExec->ReleaseSemaphore(&res->mapLock);
        } else
            IExec->DebugPrintF("clib4: '%s': its parent is not told\n", name);
    }

    if (res != NULL && program >= 0) {
        struct ReapProgram *rp = &reapPrograms[program];
        struct _clib4 *ctx = rp->rp_Ctx;

        if (ctx != NULL) {
            /* Members whose kill is under way are stopped for good, and close nothing. */
            if (others_run(rp, ctx, victim, FALSE)) {
                ULONG kept = keep_records(victim);

                if (kept != 0)
                    IExec->DebugPrintF("clib4: '%s': others of its program run, %lu files stay open\n", name, kept);
            } else
                msg->rm_LibcObjects += finish_program(msg, res, rp, ctx);
        }
    }
    IExec->ReplyMsg(&msg->rm_Msg);
}

/* The heap's blocks, from the allocator's own table, never from the list inside them. */
static void note_heap(struct _clib4 *ctx, struct Task *victim, ULONG gate, const char *name)
{
    static const char *const words[] = { "live", "stalled", "reboot-advised", "requests-left", "packet-out" };
    wmem_allocator_t *allocator = (wmem_allocator_t *) ctx->__wmem_allocator;
    void *addr[WMEM_OS_BLOCKS];
    size_t size[WMEM_OS_BLOCKS];
    char said[80];
    int n = WMEM_OS_NOTABLE, i;

    if (allocator != NULL && IExec->TypeOfMem(allocator) != 0)
        n = wmem_block_os_blocks(allocator, addr, size, WMEM_OS_BLOCKS);
    if (n < 0) {
        IExec->DebugPrintF("clib4: '%s': its heap is not noted: %s\n", name,
                           n == WMEM_OS_LOST ? "its table does not name every block" : "its allocator keeps no table");
        return;
    }
    for (i = 0; i < n; i++)
        ExecExt(IExec)->MemReportNote(IExec, MRN_ARENA | gate, victim, addr[i], size[i]);

    said[0] = '\0';
    for (i = 0; i < (int)(sizeof(words) / sizeof(words[0])); i++)
        if (gate & (MRNF_ARENA_LIVE << i)) {
            if (said[0] != '\0')
                strncat(said, ",", sizeof(said) - strlen(said) - 1);
            strncat(said, words[i], sizeof(said) - strlen(said) - 1);
        }
    IExec->DebugPrintF("clib4: '%s': %ld blocks of its heap noted, gate %s\n", name, (long) n, said[0] != '\0' ? said : "ok");
}

struct group_said {
    struct Task *victim;
    ULONG        sent;
};

static void group_say(void *data, int kind, void *addr, size_t size)
{
    struct group_said *g = data;
    ULONG what = kind == WMEM_LIVE_BLOCK ? MRN_GROUPBLOCK : kind == WMEM_LIVE_WHOLE ? MRN_GROUPLIVE | MRNF_GROUP_WHOLE : MRN_GROUPLIVE;

    ExecExt(IExec)->MemReportNote(IExec, what, g->victim, addr, size);
    g->sent++;
}

/* A block of the program's that is no heap block: all of it may be in use. */
static void group_member(struct group_said *g, void *addr, size_t size)
{
    ExecExt(IExec)->MemReportNote(IExec, MRN_GROUPBLOCK | MRNF_GROUP_NOTHEAP, g->victim, addr, size);
    ExecExt(IExec)->MemReportNote(IExec, MRN_GROUPLIVE | MRNF_GROUP_NOTHEAP, g->victim, addr, size);
    g->sent += 2;
}

/*
 * The program's heap as one group: its OS blocks, the ranges in them that may be in use, and the
 * blocks that are no heap blocks, its context and its allocator. Only a program nobody of runs is
 * walked; the report is told the gate in any case. The kernel only counts what points into it.
 * When nothing speaks against it, the program leaves the table before the group is closed, so that
 * the table names none of it. TRUE if it did.
 */
static BOOL note_group(struct _clib4 *ctx, struct Task *victim, ULONG gate, const char *name, struct ReapProgram *rp)
{
    wmem_allocator_t *allocator = (wmem_allocator_t *) ctx->__wmem_allocator;
    struct group_said g = { victim, 0 };
    BOOL cleared = FALSE;
    int n = WMEM_OS_NOTABLE;

    if ((gate & MRNF_ARENA_LIVE) == 0) {
        if (allocator != NULL && IExec->TypeOfMem(allocator) != 0)
            n = wmem_block_live_ranges(allocator, group_say, &g);
        if (n >= 0) {
            group_member(&g, ctx, sizeof(*ctx));
            group_member(&g, allocator, sizeof(*allocator));
            if (allocator->private_data != NULL)
                group_member(&g, allocator->private_data, 1);
        } else
            gate |= MRNF_GROUP_NOWALK;
    }
    if (gate == 0 && rp->rp_Finished) {
        program_clear(rp);
        cleared = TRUE;
    }
    ExecExt(IExec)->MemReportNote(IExec, MRN_GROUPEND | gate, victim, NULL, g.sent);
    IExec->DebugPrintF("clib4: '%s': its heap told as a group of %lu notes\n", name, g.sent);
    return cleared;
}

/*
 * The last of a program's kills to come here speaks for its heap. A program with any task of its
 * still running, a child that has its pr_UID included, or any kill of it that did not end cleanly
 * is noted with that said: the report then only counts the blocks.
 */
static void arena_round(struct ReapMsg *msg, int program)
{
    struct Task *victim = msg->rm_Task;
    const char *name = victim->tc_Node.ln_Name != NULL ? (const char *)victim->tc_Node.ln_Name : "?";
    struct ReapProgram *rp;
    struct _clib4 *ctx;
    ULONG flags, gate = 0;
    LONG jobs;

    if (program < 0) {
        IExec->ReplyMsg(&msg->rm_Msg);
        return;
    }
    rp = &reapPrograms[program];
    ctx = rp->rp_Ctx;
    flags = __sync_or_and_fetch(&rp->rp_Flags, msg->rm_Flags & REAP_GROUP_FLAGS);
    jobs = __sync_sub_and_fetch(&rp->rp_Jobs, 1);
    /* A kill that was never counted gives back what it took, and no kill counted since is lost. */
    if (jobs < 0)
        __sync_fetch_and_sub(&rp->rp_Jobs, jobs);
    if (jobs > 0 || ctx == NULL) {
        IExec->ReplyMsg(&msg->rm_Msg);
        return;
    }

    if (flags & RRF_STALLED)
        gate |= MRNF_ARENA_STALLED;
    if (flags & RRF_REBOOTADVISED)
        gate |= MRNF_ARENA_REBOOT;
    if (flags & RRF_REQUESTSLEFT)
        gate |= MRNF_ARENA_REQUESTS;
    if (flags & RRF_PACKETOUT)
        gate |= MRNF_ARENA_PACKET;

    /* Who runs with this context, and a member's kill counted since this one's was the last:
     * that member is stopped and on no list, and what its kill says is not in the flags yet. */
    if (others_run(rp, ctx, victim, TRUE) || rp->rp_Jobs != 0)
        gate |= MRNF_ARENA_LIVE;

    note_heap(ctx, victim, gate, name);
    if (!note_group(ctx, victim, gate, name, rp) && rp->rp_Finished)
        program_clear(rp);
    IExec->ReplyMsg(&msg->rm_Msg);
}

/* -------------------------------------------------------------------------- */
/* The client                                                                  */
/* -------------------------------------------------------------------------- */

static LONG reap_entry(struct ReapMsg *msg, int program)
{
    if (msg->rm_Round == REAP_ROUND_UNHOOK && program >= 0)
        stop_members(msg, &reapPrograms[program]);
    else if (msg->rm_Round == REAP_ROUND_ARENA)
        arena_round(msg, program);
    else
        libc_round(msg, program);

    /* The code this returns from stays until the task has gone: that ends the Forbid too. */
    IExec->Forbid();
    __sync_fetch_and_sub(&reapHelpers, 1);
    return 0;
}

/* In the kernel's reaper: nothing here waits. */
static void reap_client(struct ReapClient *client, struct ReapMsg *msg)
{
    struct Task *victim = msg->rm_Task;
    int program = program_of(victim);
    BOOL work = program >= 0;

    (void)client;
    if (work && msg->rm_Round == REAP_ROUND_UNHOOK)
        count_kill(&reapPrograms[program], msg);
    if (!work && msg->rm_Round == REAP_ROUND_LIBC && victim->tc_Node.ln_Type == NT_PROCESS)
        work = ((struct Process *)victim)->pr_ExitCode == (APTR)spawnedProcessExit;
    if (work) {
        struct TagItem tags[] = {
            { AT_Param1, (ULONG)msg },
            { AT_Param2, (ULONG)program },
            { TAG_DONE,  0 }
        };

        __sync_fetch_and_add(&reapHelpers, 1);
        if (IExec->CreateTask((CONST_STRPTR)reapName, REAP_PRIORITY, (APTR)reap_entry, REAP_STACK, tags) != NULL)
            return;
        __sync_fetch_and_sub(&reapHelpers, 1);
    }
    IExec->ReplyMsg(&msg->rm_Msg);
}

/* Who is closing a program is not closed. */
static BOOL reap_refuse(struct ReapClient *client, struct Task *task)
{
    const char *name = (const char *)task->tc_Node.ln_Name;

    (void)client;
    return name != NULL && strcmp(name, reapName) == 0;
}

void __reap_init(void)
{
    if (!ExecExtAvailable(IExec))
        return;
    reapClient.rc_Name = (CONST_STRPTR)"clib4.library";
    reapClient.rc_Reap = reap_client;
    reapClient.rc_Refuse = reap_refuse;
    reapClient.rc_Rounds = (1UL << REAP_ROUND_UNHOOK) | (1UL << REAP_ROUND_LIBC) | (1UL << REAP_ROUND_ARENA);
    ExecExt(IExec)->AddReapClient(IExec, &reapClient);
}

/* FALSE while a helper is still at it: the library stays, and the client with it. */
BOOL __reap_stop(void)
{
    if (!ExecExtAvailable(IExec))
        return TRUE;
    ExecExt(IExec)->RemReapClient(IExec, &reapClient);
    if (reapHelpers != 0) {
        ExecExt(IExec)->AddReapClient(IExec, &reapClient);
        return FALSE;
    }
    return TRUE;
}
