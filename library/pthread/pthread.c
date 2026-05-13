/*
  $Id: pthread.c,v 1.00 2022-07-18 12:09:49 clib4devs Exp $

  Copyright (C) 2014 Szilard Biro
  Copyright (C) 2018 Harry Sintonen
  Copyright (C) 2019 Stefan "Bebbo" Franke - AmigaOS 3 port

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/

#define __USE_OLD_TIMEVAL__
#ifndef _TIME_HEADERS_H
#include "time_headers.h"
#endif /* _TIME_HEADERS_H */

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

#ifndef _STDLIB_CONSTRUCTOR_H
#include "stdlib_constructor.h"
#endif /* _STDLIB_CONSTRUCTOR_H */

#include "common.h"
#include "pthread.h"
#include "../shared_library/interface.h"

int __pthread_init_func(void);
void __pthread_exit_func(void);
void __attribute__((constructor, used)) __pthread_init();
void __attribute__((destructor, used)) __pthread_exit();

struct MinList join_list;
ThreadInfo threads[PTHREAD_THREADS_MAX];
APTR thread_sem = NULL;
APTR tls_sem = NULL;
TLSKey tlskeys[PTHREAD_KEYS_MAX];

APTR timerMutex = NULL;
struct TimeRequest *timedTimerIO = NULL;
struct MsgPort *timedTimerPort = NULL;

struct Library *_DOSBase = NULL;
struct DOSIFace *_IDOS = NULL;

//
// Private common functions
//
int
_pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr, BOOL staticinit) {
    ENTER();

    if (mutex == NULL)
        return EINVAL;
    BOOL recursive = FALSE;
    SHOWPOINTER(mutex);

    if (attr)
        mutex->kind = attr->kind;
    else if (!staticinit)
        mutex->kind = PTHREAD_MUTEX_DEFAULT;

    if (mutex->kind == PTHREAD_MUTEX_RECURSIVE)
        recursive = TRUE;

    SHOWMSG("Allocating mutex");
    mutex->mutex = AllocSysObjectTags(ASOT_MUTEX, ASOMUTEX_Recursive, recursive, TAG_DONE);
    SHOWPOINTER(mutex->mutex);
    /* Check for allocation failure — NULL return on OOM would crash on
     * subsequent MutexObtain(NULL). */
    if (mutex->mutex == NULL) {
        LEAVE();
        return ENOMEM;
    }

	mutex->owner = NULL;
    mutex->incond = 0;

    LEAVE();
    return 0;
}

static void timespec_sub(struct timespec *ts1, const struct timespec *ts2, const struct timespec *ts3) {
    ts1->tv_sec = ts2->tv_sec - ts3->tv_sec;
    ts1->tv_nsec = ts2->tv_nsec - ts3->tv_nsec;
    while (ts1->tv_nsec < 0) {
        ts1->tv_sec--;
        ts1->tv_nsec += 1000000000L;
    }
    while (ts1->tv_nsec >= 1000000000L) {
        ts1->tv_sec++;
        ts1->tv_nsec -= 1000000000L;
    }
}

static void CondWaitCleanupHandler(void *arg) {
    CondWaitCleanup *ctx = (CondWaitCleanup *) arg;

    /* Remove the waiter node from the condvar's list */
    ObtainSemaphore(ctx->cond->semaphore);
    Remove((struct Node *) ctx->waiter);
    ReleaseSemaphore(ctx->cond->semaphore);

    /* Free the signal bit */
    if (ctx->waiter->sigbit != SIGB_COND_FALLBACK)
        FreeSignal(ctx->waiter->sigbit);

    /* Abort and reset per-thread timer if active (don't close — it's reusable) */
    if (ctx->timerio != NULL) {
        if (!CheckIO(ctx->timerio))
            AbortIO(ctx->timerio);
        WaitIO(ctx->timerio);
        /*
         * Mark the IORequest as "quick-complete" so that any subsequent
         * WaitIO (e.g. in StarterFunc's cleanup) returns immediately without
         * calling Remove() on the already-munged mn_Node.  SendIO() clears
         * IOF_QUICK before the next request, so this is safe to reuse.
         */
        ctx->timerio->io_Flags |= IOF_QUICK;
    }

    /* POSIX: mutex must be re-acquired when cancelled during cond_wait */
    pthread_mutex_lock(ctx->mutex);
    ctx->mutex->incond--;
}

int
_pthread_obtain_sema_timed(struct SignalSemaphore *sema, const struct timespec *abstime, int shared) {
    struct Task *task;
    ThreadInfo *inf;

    if (sema == NULL || abstime == NULL)
        return EINVAL;

    task = FindTask(NULL);
    inf = GetCurrentThreadInfo();

    for (;;) {
        ULONG ret;
        struct timespec now;

        ret = (shared == SM_SHARED) ? AttemptSemaphoreShared(sema) : AttemptSemaphore(sema);
        if (ret == TRUE)
            return 0;

        clock_gettime(CLOCK_REALTIME, &now);
        if ((now.tv_sec > abstime->tv_sec) ||
            (now.tv_sec == abstime->tv_sec && now.tv_nsec >= abstime->tv_nsec)) {
            return ETIMEDOUT;
        }

        if (inf)
            pthread_testcancel();

        if (SetSignal(0, 0) & SIGBREAKF_CTRL_C) {
            pthread_testcancel();
            Signal(task, SIGBREAKF_CTRL_C);
        }

        /* 1 tick granularity keeps implementation simple and avoids Procure/Vacate races. */
        Delay(1);
    }
}

void
_pthread_clear_threadinfo(ThreadInfo *inf) {
    D(("_pthread_clear_threadinfo: ENTER\n"));

    /* NEVER clear threads[0] - it's reserved for main thread! */
    if (inf == &threads[0]) {
        D(("_pthread_clear_threadinfo: WARNING - attempted to clear threads[0] (main thread), skipping!\n"));
        return;
    }

    /*
     * If the per-thread timer device was lazily opened and never closed
     * (e.g. the thread exited via a non-standard path, or StarterFunc's
     * cleanup was skipped), close it here before zeroing the structure.
     *
     * This is critical when kernel "munge" is active: without this step
     * the slot is recycled with timerOpen=FALSE but the inline timerPort /
     * timerIO structs still contain stale pointers (timerPort.mp_SigTask
     * → dead task, io_Message.mn_ReplyPort → stale port).  The next
     * OpenTimerDevice on the recycled slot re-initialises those fields,
     * but the kernel can deliver a reply to the old, munged address before
     * the initialisation completes, triggering a DSI at 0xCCCCCCxx.
     */
    if (inf->timerOpen) {
        D(("_pthread_clear_threadinfo: closing stale timer device for slot (timerOpen=TRUE)\n"));
        if (!CheckIO((struct IORequest *)&inf->timerIO))
            AbortIO((struct IORequest *)&inf->timerIO);
        WaitIO((struct IORequest *)&inf->timerIO);
        CloseDevice((struct IORequest *)&inf->timerIO);
        if (inf->timerPort.mp_SigBit != SIGB_TIMER_FALLBACK)
            FreeSignal(inf->timerPort.mp_SigBit);
        inf->timerOpen = FALSE;
    }

    D(("_pthread_clear_threadinfo: clearing thread (task value suppressed)\n"));
    memset(inf, 0, sizeof(ThreadInfo));
    NewMinList(&inf->cleanup);
    inf->status = THREAD_STATE_IDLE;
    inf->can_exit = 0;
    D(("_pthread_clear_threadinfo: EXIT\n"));
}

int
_pthread_cond_timedwait(pthread_cond_t *cond, pthread_mutex_t *mutex, const struct timespec *abstime, BOOL relative) {
    CondWaiter waiter;
    BYTE signal;
    ULONG sigs = SIGBREAKF_CTRL_C;
    struct Task *task;
    ThreadInfo *inf;
    clock_t clock_type = CLOCK_REALTIME;

    if (cond == NULL || mutex == NULL) {
        return EINVAL;
    }

    clock_type = cond->condattr.clock_type;

    /* Check for supported clock type */
    if ((clock_type & ~(CLOCK_MONOTONIC | CLOCK_REALTIME | CLOCK_MONOTONIC_RAW)) != 0) {
        return EINVAL;
    }

    // initialize static conditions
    if (SemaphoreIsInvalid(cond->semaphore)) {
        MutexObtain(thread_sem);
        pthread_cond_init(cond, NULL);
        MutexRelease(thread_sem);
    }

    task = FindTask(NULL);
    inf = GetCurrentThreadInfo();

    if (abstime) {
        // Compute relative time BEFORE sending the timer request.
        // This way, if the deadline has already passed we can return
        // ETIMEDOUT immediately without any IO overhead.
        struct timespec rel_time;
        if (!relative) {
            struct timespec starttime;
            // absolute time has to be converted to relative
            // First normalize abstime in case tv_nsec is out of range
            // (e.g. 32-bit overflow from caller's arithmetic)
            struct timespec normabs = *abstime;
            // Use unsigned arithmetic to correctly handle 32-bit signed overflow.
            // When a caller does tv_nsec += ms * 1000000 and the result exceeds
            // INT32_MAX, the signed value wraps negative. Casting to unsigned long
            // recovers the mathematically correct (pre-overflow) value.
            {
                unsigned long uns_nsec = (unsigned long)normabs.tv_nsec;
                normabs.tv_sec += (long)(uns_nsec / 1000000000UL);
                normabs.tv_nsec = (long)(uns_nsec % 1000000000UL);
            }
            clock_gettime(clock_type, &starttime);
            timespec_sub(&rel_time, &normabs, &starttime);
            // Normalize tv_nsec to [0, 999999999] range
            if (rel_time.tv_nsec >= 1000000000L) {
                rel_time.tv_sec += rel_time.tv_nsec / 1000000000L;
                rel_time.tv_nsec = rel_time.tv_nsec % 1000000000L;
            }
            if (rel_time.tv_nsec < 0) {
                rel_time.tv_sec--;
                rel_time.tv_nsec += 1000000000L;
            }
            // Check if the time is already in the past
            if (rel_time.tv_sec < 0 || (rel_time.tv_sec == 0 && rel_time.tv_nsec <= 0)) {
                return ETIMEDOUT;
            }
        } else {
            // relative time - use abstime directly
            // Check if the time is valid
            if (abstime->tv_sec < 0 || (abstime->tv_sec == 0 && abstime->tv_nsec <= 0)) {
                return ETIMEDOUT;
            }
            rel_time = *abstime;
        }

        // Lazy-initialize per-thread timer device (opened once, reused across calls)
        if (!inf->timerOpen) {
            if (!OpenTimerDevice((struct IORequest *) &inf->timerIO, &inf->timerPort, task)) {
                return EINVAL;
            }
            inf->timerOpen = TRUE;
        }
        // prepare the device command and send it
        inf->timerIO.Request.io_Command = TR_ADDREQUEST;
        inf->timerIO.Request.io_Flags = 0;
        TIMESPEC_TO_OLD_TIMEVAL(&inf->timerIO.Time, &rel_time);
        sigs |= (1 << inf->timerPort.mp_SigBit);
        // Clear stale timer signal and drain any leftover messages from previous use
        SetSignal(0, 1 << inf->timerPort.mp_SigBit);
        while (GetMsg(&inf->timerPort)) { /* drain */ }
        SendIO((struct IORequest *) &inf->timerIO);
    }
    // prepare a waiter node
    waiter.task = task;
    signal = AllocSignal(-1);
    if (signal == -1) {
        signal = SIGB_COND_FALLBACK;
        SetSignal(0, SIGF_COND_FALLBACK);
    }
    waiter.sigbit = signal;
    sigs |= 1 << waiter.sigbit;
    // add it to the end of the list
    ObtainSemaphore(cond->semaphore);
    AddTail((struct List *) cond->waiters, (struct Node *) &waiter);
    ReleaseSemaphore(cond->semaphore);
    // wait for the condition to be signalled or the timeout
    mutex->incond++;
    pthread_mutex_unlock(mutex);

    // Push cancellation cleanup to remove waiter node if thread is cancelled during Wait
    CondWaitCleanup cleanup_ctx;
    cleanup_ctx.cond = cond;
    cleanup_ctx.mutex = mutex;
    cleanup_ctx.waiter = &waiter;
    cleanup_ctx.timerio = abstime ? (struct IORequest *) &inf->timerIO : NULL;
    pthread_cleanup_push(CondWaitCleanupHandler, &cleanup_ctx);

    sigs = Wait(sigs);

    pthread_cleanup_pop(0); // don't execute — we handle cleanup below

    pthread_mutex_lock(mutex);
    mutex->incond--;
    // remove the node from the list
    ObtainSemaphore(cond->semaphore);
    Remove((struct Node *) &waiter);
    ReleaseSemaphore(cond->semaphore);

    if (waiter.sigbit != SIGB_COND_FALLBACK)
        FreeSignal(waiter.sigbit);

    if (abstime) {
        // Reset the timer IO for reuse (abort if still pending, then wait for completion)
        if (!CheckIO((struct IORequest *) &inf->timerIO))
            AbortIO((struct IORequest *) &inf->timerIO);
        WaitIO((struct IORequest *) &inf->timerIO);
        /* See comment in _pthread_obtain_sema_timed: prevent double-Remove(). */
        inf->timerIO.Request.io_Flags |= IOF_QUICK;

        // did we timeout?
        if (sigs & (1 << inf->timerPort.mp_SigBit))
            return ETIMEDOUT;
        else if (sigs & SIGBREAKF_CTRL_C) {
            pthread_testcancel();
            // Re-Enable CTRL-C in case a signal handler is installed
            Signal(task, SIGBREAKF_CTRL_C);
        }
    } else {
        if (sigs & SIGBREAKF_CTRL_C) {
            pthread_testcancel();
            // Re-Enable CTRL-C in case a signal handler is installed
            Signal(task, SIGBREAKF_CTRL_C);
        }
    }

    return 0;
}

int
_pthread_cond_broadcast(pthread_cond_t *cond, BOOL onlyfirst) {
    CondWaiter *waiter;

    if (cond == NULL)
        return EINVAL;

    // initialize static conditions
    if (SemaphoreIsInvalid(cond->semaphore)) {
        MutexObtain(thread_sem);
        pthread_cond_init(cond, NULL);
        MutexRelease(thread_sem);
    }

    // signal the waiting threads
    ObtainSemaphore(cond->semaphore);
    ForeachNode(cond->waiters, waiter) {
        Signal(waiter->task, 1 << waiter->sigbit);
        if (onlyfirst) break;
    }
    ReleaseSemaphore(cond->semaphore);

    return 0;
}

//
// Constructors, destructors
//
// Store the previous value (before pthread runtime takes over)
static ThreadInfo *old_tls = NULL;

int __pthread_init_func(void) {
    pthread_t i;
    SHOWMSG("[__pthread_init_func :] Pthread __pthread_init_func called.\n");

    memset(&threads, 0, sizeof(threads));
    NewMinList(&join_list);
    thread_sem = AllocSysObjectTags(ASOT_MUTEX, ASOMUTEX_Recursive, TRUE, TAG_DONE);
    tls_sem = AllocSysObjectTags(ASOT_MUTEX, ASOMUTEX_Recursive, TRUE, TAG_DONE);

    old_tls = get_tls_register();

    // reserve ID 0 for the main thread
    ThreadInfo *inf = &threads[0];

    inf->thread_id = 0;  /* Main thread has ID 0 */
    inf->task = (struct Process *) FindTask(NULL);
    inf->status = THREAD_STATE_RUNNING;
    NewMinList(&inf->cleanup);

    /* Allocate signals for main thread */
    inf->cancel_signal = AllocSignal(-1);
    if (inf->cancel_signal == -1) {
        inf->cancel_signal_mask = SIGBREAKF_CTRL_C;
    } else {
        inf->cancel_signal_mask = 1L << inf->cancel_signal;
    }

    inf->join_signal = AllocSignal(-1);
    if (inf->join_signal == -1) {
        inf->join_signal_mask = SIGF_PARENT;
    } else {
        inf->join_signal_mask = 1L << inf->join_signal;
    }

    timerMutex = AllocSysObjectTags(ASOT_MUTEX, ASOMUTEX_Recursive, TRUE, TAG_DONE);

    timedTimerPort = AllocSysObjectTags(ASOT_PORT, TAG_DONE);
    timedTimerIO = AllocSysObjectTags(ASOT_IOREQUEST,
                                      ASOIOR_ReplyPort, timedTimerPort,
                                      ASOIOR_Size, sizeof(struct TimeRequest),
                                      TAG_DONE);

    OpenDevice(TIMERNAME, UNIT_WAITUNTIL, (struct IORequest *) timedTimerIO, 0);

    set_tls_register(inf);

    /* Mark all threads as IDLE */
    for (i = PTHREAD_FIRST_THREAD_ID; i < PTHREAD_THREADS_MAX; i++) {
        inf = &threads[i];
        inf->status = THREAD_STATE_IDLE;
        inf->cancel_signal = -1; /* No signal allocated yet */
        inf->join_signal = -1;
        inf->join_signal_mask = 0;
        inf->join_thread_id = 0;
    }


    return TRUE;
}

void __pthread_exit_func(void) {
    pthread_t i;
    ThreadInfo *inf;
    struct DOSIFace *IDOS = _IDOS;
    SHOWMSG("[__pthread_exit_func :] Pthread __pthread_exit_func called.\n");

    // if we don't do this we can easily end up with unloaded code being executed
    for (i = PTHREAD_FIRST_THREAD_ID; i < PTHREAD_THREADS_MAX; i++) {
        inf = &threads[i];
        if (inf->detached) {
            // TODO longer delay between retries?
            while (inf->task)
                Delay(1);
        } else {
            /* Join any non-idle joinable thread, not just RUNNING.
             * Threads in JOINING/WAITING/TERMINATING states also need cleanup. */
            if (inf->status != THREAD_STATE_IDLE)
                pthread_join(i, NULL);
        }
    }

	MutexObtain(thread_sem);
	inf = &threads[0];

	/* Close per-thread timer device for main thread if it was lazily opened */
	if (inf->timerOpen) {
		if (!CheckIO((struct IORequest *)&inf->timerIO))
			AbortIO((struct IORequest *)&inf->timerIO);
		WaitIO((struct IORequest *)&inf->timerIO);
		CloseDevice((struct IORequest *)&inf->timerIO);
		if (inf->timerPort.mp_SigBit != SIGB_TIMER_FALLBACK)
			FreeSignal(inf->timerPort.mp_SigBit);
		inf->timerOpen = FALSE;
	}

	if (inf->cancel_signal != -1) {
		D(("_pthread_clear_threadinfo: Freeing cancel signal %d\n", inf->cancel_signal));
		FreeSignal(inf->cancel_signal);
		inf->cancel_signal = -1;
	}
	if (inf->join_signal != -1) {
		D(("_pthread_clear_threadinfo: Freeing join signal %d\n", inf->join_signal));
		FreeSignal(inf->join_signal);
		inf->join_signal = -1;
	}
	MutexRelease(thread_sem);

    if (thread_sem) {
        FreeSysObject(ASOT_MUTEX, thread_sem);
        thread_sem = NULL;
    }
    if (tls_sem) {
        FreeSysObject(ASOT_MUTEX, tls_sem);
        tls_sem = NULL;
    }
    if (timerMutex) {
        FreeSysObject(ASOT_MUTEX, timerMutex);
        timerMutex = NULL;
    }
    if (timedTimerIO) {
        CloseDevice((struct IORequest *) timedTimerIO);
        FreeSysObject(ASOT_IOREQUEST, timedTimerIO);
        timedTimerIO = NULL;
    }
    if (timedTimerPort) {
        FreeSysObject(ASOT_PORT, timedTimerPort);
        timedTimerPort = NULL;
    }

    set_tls_register(NULL);
}


PTHREAD_CONSTRUCTOR(__pthread_init) {
    ENTER();
    SHOWMSG("[__pthread_init :] Pthread constructor called.\n");
    _DOSBase = OpenLibrary("dos.library", MIN_OS_VERSION);
    if (_DOSBase) {
        _IDOS = (struct DOSIFace *) GetInterface((struct Library *) _DOSBase, "main", 1, NULL);
        if (!_IDOS) {
            CloseLibrary(_DOSBase);
            _DOSBase = NULL;
        }
        else
            __pthread_init_func();
    }
    LEAVE();
}

PTHREAD_DESTRUCTOR(__pthread_exit) {
    ENTER();
    SHOWMSG("[__pthread_exit :] Pthread destructor called.\n");

    __pthread_exit_func();

    if (_IDOS != NULL) {
        DropInterface((struct Interface *) _IDOS);
        _IDOS = NULL;
    }

    if (_DOSBase != NULL) {
        CloseLibrary(_DOSBase);
        _DOSBase = NULL;
    }

    LEAVE();
}
