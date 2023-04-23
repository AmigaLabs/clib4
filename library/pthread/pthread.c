/*
  $Id: pthread.c,v 1.00 2022-07-18 12:09:49 clib2devs Exp $

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

int __pthread_init_func(void);
void __pthread_exit_func(void);
void __attribute__((constructor, used)) __pthread_init();
void __attribute__((destructor, used)) __pthread_exit();

ThreadInfo threads[PTHREAD_THREADS_MAX];
struct SignalSemaphore thread_sem;
TLSKey tlskeys[PTHREAD_KEYS_MAX];
struct SignalSemaphore tls_sem;

//
// Private common functions
//

int
_pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr, BOOL staticinit) {
    if (mutex == NULL)
        return EINVAL;

    if (attr)
        mutex->kind = attr->kind;
    else if (!staticinit)
        mutex->kind = PTHREAD_MUTEX_DEFAULT;
    InitSemaphore(&mutex->semaphore);
    mutex->incond = 0;

    return 0;
}

int
_pthread_obtain_sema_timed(struct SignalSemaphore *sema, const struct timespec *abstime, int shared) {
    struct MsgPort msgport;
    struct SemaphoreMessage msg;
    struct TimeRequest timerio;
    struct Task *task;
    struct Message *m1, *m2;

    task = FindTask(NULL);

    if (!OpenTimerDevice((struct IORequest *) &timerio, &msgport, task)) {
        CloseTimerDevice((struct IORequest *) &timerio);
        return EINVAL;
    }

    timerio.Request.io_Command = TR_ADDREQUEST;
    timerio.Request.io_Flags = 0;
    TIMESPEC_TO_OLD_TIMEVAL(&timerio.Time, abstime);
    //if (!relative)
    {
        struct TimeVal starttime;
        // absolute time has to be converted to relative
        // GetSysTime can't be used due to the timezone offset in abstime
        gettimeofday((struct timeval *)&starttime, NULL);
        timersub(&timerio.Time, &starttime, &timerio.Time);
        if (!timerisset(&timerio.Time)) {
            CloseTimerDevice((struct IORequest *) &timerio);
            return ETIMEDOUT;
        }
    }
    SendIO((struct IORequest *) &timerio);

    msg.ssm_Message.mn_Node.ln_Type = NT_MESSAGE;
    msg.ssm_Message.mn_Node.ln_Name = (char *) shared;
    msg.ssm_Message.mn_ReplyPort = &msgport;
    Procure(sema, &msg);

    WaitPort(&msgport);
    m1 = GetMsg(&msgport);
    m2 = GetMsg(&msgport);
    if (m1 == &timerio.Request.io_Message || m2 == &timerio.Request.io_Message)
        Vacate(sema, &msg);

    CloseTimerDevice((struct IORequest *) &timerio);

    if (msg.ssm_Semaphore == NULL)
        return ETIMEDOUT;

    return 0;
}

void
_pthread_clear_threadinfo(ThreadInfo *inf) {
    memset(inf, 0, sizeof(ThreadInfo));
    inf->status = THREAD_STATE_IDLE;
}

int
_pthread_cond_timedwait(pthread_cond_t *cond, pthread_mutex_t *mutex, const struct timespec *abstime, BOOL relative) {
    CondWaiter waiter;
    BYTE signal;
    ULONG sigs = SIGBREAKF_CTRL_C;
    struct MsgPort timermp;
    struct TimeRequest timerio;
    struct Task *task;

    if (cond == NULL || mutex == NULL)
        return EINVAL;

    // initialize static conditions
    if (SemaphoreIsInvalid(&cond->semaphore))
        pthread_cond_init(cond, NULL);

    task = FindTask(NULL);

    if (abstime) {
        // open timer.device
        if (!OpenTimerDevice((struct IORequest *) &timerio, &timermp, task)) {
            CloseTimerDevice((struct IORequest *) &timerio);
            return EINVAL;
        }

        // prepare the device command and send it
        timerio.Request.io_Command = TR_ADDREQUEST;
        timerio.Request.io_Flags = 0;
        TIMESPEC_TO_OLD_TIMEVAL(&timerio.Time, abstime);
        if (!relative) {
            struct TimeVal starttime;
            // absolute time has to be converted to relative
            // GetSysTime can't be used due to the timezone offset in abstime
            gettimeofday((struct timeval *)&starttime, NULL);
            timersub(&timerio.Time, &starttime, &timerio.Time);
            if (!timerisset(&timerio.Time)) {
                CloseTimerDevice((struct IORequest *) &timerio);
                return ETIMEDOUT;
            }
        }
        sigs |= (1 << timermp.mp_SigBit);
        SendIO((struct IORequest *) &timerio);
    }

    // prepare a waiter node
    waiter.task = task;
    signal = AllocSignal(-1);
    if (signal == -1) {
        signal = SIGB_COND_FALLBACK;
        SetSignal(SIGF_COND_FALLBACK, 0);
    }
    waiter.sigbit = signal;
    sigs |= 1 << waiter.sigbit;

    // add it to the end of the list
    ObtainSemaphore(&cond->semaphore);
    AddTail((struct List *) &cond->waiters, (struct Node *) &waiter);
    ReleaseSemaphore(&cond->semaphore);

    // wait for the condition to be signalled or the timeout
    mutex->incond++;
    pthread_mutex_unlock(mutex);
    sigs = Wait(sigs);
    pthread_mutex_lock(mutex);
    mutex->incond--;

    // remove the node from the list
    ObtainSemaphore(&cond->semaphore);
    Remove((struct Node *) &waiter);
    ReleaseSemaphore(&cond->semaphore);

    if (waiter.sigbit != SIGB_COND_FALLBACK)
        FreeSignal(waiter.sigbit);

    if (abstime) {
        // clean up the TimeRequest
        CloseTimerDevice((struct IORequest *) &timerio);

        // did we timeout?
        if (sigs & (1 << timermp.mp_SigBit))
            return ETIMEDOUT;
        else if (sigs & SIGBREAKF_CTRL_C)
            pthread_testcancel();
    } else {
        if (sigs & SIGBREAKF_CTRL_C)
            pthread_testcancel();
    }

    return 0;
}

int
_pthread_cond_broadcast(pthread_cond_t *cond, BOOL onlyfirst) {
    CondWaiter *waiter;

    if (cond == NULL)
        return EINVAL;

    // initialize static conditions
    if (SemaphoreIsInvalid(&cond->semaphore))
        pthread_cond_init(cond, NULL);

    // signal the waiting threads
    ObtainSemaphore(&cond->semaphore);
    ForeachNode(&cond->waiters, waiter) {
        Signal(waiter->task, 1 << waiter->sigbit);
        if (onlyfirst) break;
    }
    ReleaseSemaphore(&cond->semaphore);

    return 0;
}

//
// Constructors, destructors
//

int __pthread_init_func(void) {
    pthread_t i;

    memset(&threads, 0, sizeof(threads));
    InitSemaphore(&thread_sem);
    InitSemaphore(&tls_sem);

    // reserve ID 0 for the main thread
    ThreadInfo *inf = &threads[0];
    inf->task = FindTask(NULL);
    inf->status = THREAD_STATE_RUNNING;
    NewMinList(&inf->cleanup);

    /* Mark all threads as IDLE */
    for (i = PTHREAD_FIRST_THREAD_ID; i < PTHREAD_THREADS_MAX; i++) {
        inf = &threads[i];
        inf->status = THREAD_STATE_IDLE;
    }

    return TRUE;
}

void __pthread_exit_func(void) {
    pthread_t i;
    ThreadInfo *inf;

    // if we don't do this we can easily end up with unloaded code being executed
    for (i = PTHREAD_FIRST_THREAD_ID; i < PTHREAD_THREADS_MAX; i++) {
        inf = &threads[i];
        if (inf->detached) {
            // TODO longer delay between retries?
            while (inf->task)
                Delay(1);
        } else {
            if (inf->status == THREAD_STATE_RUNNING)
                pthread_join(i, NULL);
        }
    }
}

CLIB_CONSTRUCTOR(__pthread_init) {
    ENTER();

    __pthread_init_func();

    LEAVE();

    //return !__pthread_init_func();
}

CLIB_DESTRUCTOR(__pthread_exit) {
    ENTER();

    __pthread_exit_func();

    LEAVE();
}
