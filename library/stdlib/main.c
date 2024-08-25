/*
 * $Id: stdlib_main.c,v 2.00 2023-05-17 14:09:00 clib4devs Exp $
*/

#ifndef EXEC_EXECBASE_H
#include <exec/execbase.h>
#endif /* EXEC_EXECBASE_H */

#include <setjmp.h>
#include <string.h>
#include <ctype.h>

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

#ifndef _SHM_HEADERS_H
#include "shm_headers.h"
#endif /* _SHM_HEADERS_H */

#ifndef _TIME_HEADERS_H
#include "time_headers.h"
#endif /* _TIME_HEADERS_H */

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

#include <proto/elf.h>

#include "../shared_library/clib4.h"
#include "uuid.h"

/* These CTORS/DTORS are clib4's one and they are different than that one received
 * from crtbegin. They are needed because we need to call clib4 constructors as well
 */
static void (*__CTOR_LIST__[1])(void) __attribute__((section(".ctors")));
static void (*__DTOR_LIST__[1])(void) __attribute__((section(".dtors")));
extern int main(int arg_c, char **arg_v);
static void shared_obj_init(struct _clib4 *__clib4, BOOL init);

struct envHookData {
    uint32 env_size;
    uint32 allocated_size;
    struct _clib4 *r;
};

static char *empty_env[1] = {NULL};

static uint32
copyEnvironment(struct Hook *hook, struct envHookData *ehd, struct ScanVarsMsg *message) {
    DECLARE_UTILITYBASE();

    if (Strlen(message->sv_GDir) <= 4) {
        if (ehd->env_size == ehd->allocated_size) {
            if (!(ehd->r->__environment = realloc(ehd->r->__environment, ehd->allocated_size + 1024 * sizeof(char *)))) {
                return 1;
            }
            ClearMem((char *) ehd->r->__environment + ehd->allocated_size, 1024 * sizeof(char *));
            ehd->allocated_size += 1024 * sizeof(char *);
        }

        char **env = (char **) hook->h_Data;
        uint32 size = Strlen(message->sv_Name) + 1 + message->sv_VarLen + 1 + 1;
        char *buffer = (char *) AllocVecTags(size, AVT_Type, MEMF_SHARED, TAG_DONE);
        if (buffer == NULL) {
            return 1;
        }

        ++ehd->env_size;
        SNPrintf(buffer, size - 1, "%s=%s", message->sv_Name, message->sv_Var);
        *env = buffer;
        env++;
        hook->h_Data = env;
    }
    return 0;
}

static void
makeEnvironment(struct _clib4 *__clib4) {
    char varbuf[8];
    uint32 flags = 0;
    size_t environ_size = 1024 * sizeof(char *);

    if (GetVar("EXEC_IMPORT_LOCAL", varbuf, sizeof(varbuf), GVF_LOCAL_ONLY) > 0) {
        flags = GVF_LOCAL_ONLY;
    }

    __clib4->__environment = (char **) calloc(environ_size, 1);
    if (!__clib4->__environment)
        return;

    flags |= GVF_SCAN_TOPLEVEL;
    struct Hook hook;
    hook.h_Entry = (void *) copyEnvironment;
    hook.h_Data = __clib4->__environment;
    struct envHookData ehd = {1, environ_size, __clib4};
    ScanVars(&hook, flags, &ehd);
}


static void
shared_obj_init(struct _clib4 *__clib4, BOOL init) {
    ENTER();

    struct ElfIFace *IElf = __clib4->IElf;

    BPTR segment_list = GetProcSegList(NULL, GPSLF_RUN | GPSLF_SEG);
    Elf32_Handle hSelf = (Elf32_Handle) NULL;
    if (segment_list != BZERO) {
        int ret = GetSegListInfoTags(segment_list, GSLI_ElfHandle, &hSelf, TAG_DONE);
        if (ret == 1) {
            if (hSelf != NULL) {
                /* Trigger the constructors, etc. in the shared objects linked to this binary. */
                InitSHLibs(hSelf, init);
            } else {
                SHOWMSG("hSelf == NULL!");
            }
        } else {
            SHOWMSG("GetSegListInfoTags fail!");
        }
    } else {
        SHOWMSG("GetProcSegList return ZERO!");
    }

    LEAVE();
}

static void
_start_ctors(void (*__CTOR_LIST__[])(void)) {
    int i = 0;

    while (__CTOR_LIST__[i + 1]) {
        i++;
    }
    SHOWVALUE(i);
    while (i > 0) {
        D(("Calling ctor %ld", i));
        __CTOR_LIST__[i--]();
    }
}

static void
_end_ctors(void (*__DTOR_LIST__[])(void)) {
    int i = 1;

    while (__DTOR_LIST__[i]) {
        D(("Calling dtor %ld", i));
        __DTOR_LIST__[i++]();
    }
}

static int
call_main(
        char *argstr,
        int arglen,
        int (*start_main)(int, char **),
        void (*__EXT_CTOR_LIST__[])(void),
        void (*__EXT_DTOR_LIST__[])(void),
        struct _clib4 *__clib4) {
    volatile LONG saved_io_err;

    ENTER();
    /* This plants the return buffer for _exit(). */
    if (setjmp(__clib4->__exit_jmp_buf) != 0) {
        D(("Back from longjmp"));
        goto out;
    }

    SHOWMSG("Initialize shared objects");
    shared_obj_init(__clib4, TRUE);

    SHOWMSG("Now invoking constructors");
    /* Go through the constructor list */
    _start_ctors(__EXT_CTOR_LIST__);
    SHOWMSG("Constructors executed correctly. Calling start_main()");

    /* Set __current_path_name to a valid value */
    UBYTE current_dir_name[256] = {0};
    struct Process *me = (struct Process *) FindTask(NULL);
    if (NameFromLock(me->pr_CurrentDir, (STRPTR) current_dir_name, sizeof(current_dir_name))) {
        __set_current_path((const char *) current_dir_name);
    }

    D(("Call start_main with %ld parameters", __clib4->__argc));
    /* After all these preparations, get this show on the road... */
    exit(start_main(__clib4->__argc, __clib4->__argv));
    SHOWMSG("Done. Exit from start_main()");

out:
    /* Save the current IoErr() value in case it is needed later. */
    saved_io_err = IoErr();

    /* From this point on, don't worry about ^C checking any more. */
    __clib4->__check_abort_enabled = FALSE;

    SHOWMSG("Flush all files");
    /* Dump all currently unwritten data, especially to the console. */
    __flush_all_files(__clib4, -1);

    /* If one of the destructors drops into exit(), either directly
       or through a failed assert() call, processing will resume with
       the next following destructor. */
    (void) setjmp(__clib4->__exit_jmp_buf);
    SHOWMSG("Called setjmp(__clib4->__exit_jmp_buf)");

    /* Go through the destructor list */
    SHOWMSG("invoking external destructors in reverse order");
    _end_ctors(__EXT_DTOR_LIST__);

    SHOWMSG("Close shared objects");
    shared_obj_init(__clib4, FALSE);

    SHOWMSG("done.");

    /* Restore the IoErr() value before we return. */
    SetIoErr(saved_io_err);

    RETURN(__clib4->__exit_value);
    return __clib4->__exit_value;
}

int
_main(
        char *argstr,
        int arglen,
        int (*start_main)(int, char **),
        void (*__EXT_CTOR_LIST__[])(void),
        void (*__EXT_DTOR_LIST__[])(void)) {
    struct WBStartup *sms = NULL;
    struct Process *me;
    int rc = RETURN_FAIL;
    struct _clib4 *__clib4 = NULL;
    uint32 pid = GetPID(0, GPID_PROCESS);
    struct Clib4Resource *res = (APTR) OpenResource(RESOURCE_NAME);

    DECLARE_UTILITYBASE();

    /* Pick up the Workbench startup message, if available. */
    me = (struct Process *) FindTask(NULL);
    if (!me->pr_CLI) {
        struct MsgPort *mp = &me->pr_MsgPort;
        WaitPort(mp);
        sms = (struct WBStartup *) GetMsg(mp);
    }

    /* If all libraries are opened correctly we can initialize clib4 reent structure */
    D(("Initialize clib4 reent structure"));
    /* Initialize global structure */
    __clib4 = (struct _clib4 *) AllocVecTags(sizeof(struct _clib4),
                                             AVT_Type, MEMF_SHARED,
                                             AVT_ClearWithValue, 0,
                                             TAG_DONE);
    if (__clib4 == NULL) {
        Forbid();
        ReplyMsg(&sms->sm_Message);
        return -1;
    }

    reent_init(__clib4);
    __clib4->processId = pid;

    /* Set the current task pointer */
    __clib4->self = me;

    if (res) {
        size_t iter = 0;
        void *item;
        while (hashmap_iter(res->children, &iter, &item)) {
            const struct Clib4Node *node = item;
            if (node->pid == pid) {
                __clib4->uuid = node->uuid;
                D(("__clib4->uuid ) %s\n", __clib4->uuid));
                break;
            }
        }
    }

    /* Set _clib4 pointer into process pr_UID
     * This field is copied to any spawned process created by this exe and/or its children
     */
    me->pr_UID = (uint32) __clib4;
    //SetOwnerInfoTags(OI_ProcessInput, 0, OI_OwnerUID, __clib4, TAG_END);

    __clib4->__WBenchMsg = sms;

    /* After reent structure we can call clib4 constructors */
    SHOWMSG("Calling clib4 ctors");
    _start_ctors(__CTOR_LIST__);
    SHOWMSG("Done. All constructors called");

    /* Copy environment variables into clib4 reent structure */
    makeEnvironment(__clib4);
    if (!__clib4->__environment) {
        __clib4->__environment = empty_env;
        __clib4->__environment_allocated = FALSE;
    }
    else
        __clib4->__environment_allocated = TRUE;

    /* Set default terminal mode to "amiga-clib4" if not set */
    char term_buffer[FILENAME_MAX] = {0};
    LONG term_len = GetVar("TERM", (STRPTR) term_buffer, FILENAME_MAX, 0);
    if (term_len <= 0) {
        Strlcpy(term_buffer, "amiga-clib4", FILENAME_MAX);
        SetVar("TERM", term_buffer, -1, GVF_LOCAL_ONLY);
    }

    /* The following code will be executed if the program is to keep
       running in the shell or was launched from Workbench. */
    int oldPriority = me->pr_Task.tc_Node.ln_Pri;

    /* Change the task priority, if requested. */
    if (-128 <= __clib4->__priority && __clib4->__priority <= 127)
        SetTaskPri((struct Task *) me, __clib4->__priority);

    /* We can enable check abort now */
    __clib4->__check_abort_enabled = TRUE;

    /* At this point exe is fully initialized */
    __clib4->__fully_initialized = TRUE;

    SHOWMSG("Call Main");
    /* We have enough room to make the call or just don't care. */
    rc = call_main(argstr, arglen, start_main, __EXT_CTOR_LIST__, __EXT_DTOR_LIST__, __clib4);

    /* Restore the task priority. */
    SetTaskPri((struct Task *) me, oldPriority);

    /* Free environment memory */
    if (__clib4->__environment_allocated) {
        free(__clib4->__environment);
        __clib4->__environment = NULL;
    }

    /* Check for getrandom fd */
    if (__clib4->randfd[0] >= 0) {
        SHOWMSG("Closing randfd[0]");
        close(__clib4->randfd[0]);
    }

    if (__clib4->randfd[1] >= 0) {
        SHOWMSG("Closing randfd[1]");
        close(__clib4->randfd[1]);
    }

    SHOWMSG("Calling clib4 dtors");
    _end_ctors(__DTOR_LIST__);
    SHOWMSG("Done. All destructors called");

    SHOWMSG("Calling reent_exit on _clib4");
    reent_exit(__clib4, FALSE);
    SHOWMSG("Done");

    if (sms) {
        Forbid();
        ReplyMsg(&sms->sm_Message);
    }

    SHOWMSG("Exit from _main");

    return rc;
}
