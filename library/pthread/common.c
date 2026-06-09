/*
  $Id: common.c,v 1.00 2022-07-18 12:09:49 clib4devs Exp $

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

#ifndef _TIME_HEADERS_H
#include "time_headers.h"
#endif /* _TIME_HEADERS_H */

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

#include "common.h"
#include "pthread.h"

register ThreadInfo * __tls_reg asm(TLS_REGISTER);

//
// Helper functions
//
int SemaphoreIsInvalid(struct SignalSemaphore *sem) {
    return (!sem || sem->ss_Link.ln_Type != NT_SIGNALSEM || sem->ss_WaitQueue.mlh_Tail != NULL);
}

int SemaphoreIsMine(struct SignalSemaphore *sem) {
    struct Task *me = FindTask(NULL);
    return (sem && sem->ss_NestCount > 0 && sem->ss_Owner == me);
}

int MutexIsMine(pthread_mutex_t *mutex) {
    struct Task *me = FindTask(NULL);
    return (mutex && mutex->owner == me);
}

ThreadInfo *GetThreadInfo(pthread_t thread) {
    // TODO: more robust error handling?
    if (thread < PTHREAD_THREADS_MAX)
        return &threads[thread];

    return NULL;
}

void set_tls_register(ThreadInfo *ti) {
    __tls_reg = ti;
}

ThreadInfo *get_tls_register(void) {
    return __tls_reg;
}

ThreadInfo *GetCurrentThreadInfo() {
    ThreadInfo *inf = __tls_reg;
    struct Task *task = FindTask(NULL);

    /* Validate: the TLS register (r2) must point inside the threads array
     * AND the stored task pointer must match the calling task.
     * On the main thread, register r2 may be clobbered by the ABI (TOC
     * pointer) so it can contain a non-NULL but invalid pointer.
     * Even when r2 looks valid, it may reference a different thread's
     * ThreadInfo if r2 was clobbered to a value that happens to fall
     * inside the array.  The task check catches this case. */
    if (inf >= &threads[0] && inf < &threads[PTHREAD_THREADS_MAX]
        && (struct Task *)inf->task == task) {
        return inf;
    }

    /* Fallback: look up by task pointer.
     * This handles the main thread case where r2 is not set or has been
     * overwritten by the compiler/system, and any case where the fast
     * path validation failed above. */
    for (int i = 0; i < PTHREAD_THREADS_MAX; i++) {
        if ((struct Task *)threads[i].task == task && threads[i].status != THREAD_STATE_IDLE) {
            return &threads[i];
        }
    }

    return NULL;
}

pthread_t GetThreadId(struct Task *task) {
    pthread_t i;

    /* When searching for an empty slot (task==NULL), start from
     * PTHREAD_FIRST_THREAD_ID to preserve slot 0 for the main thread.
     * When searching for a specific task, start from 0 so that the main
     * thread stored in threads[0] can also be found (e.g. pthread_self()
     * called from main). */
    pthread_t start = (task == NULL) ? PTHREAD_FIRST_THREAD_ID : 0;

    for (i = start; i < PTHREAD_THREADS_MAX; i++) {
        if (threads[i].task == (struct Process *) task) {
            /* When searching for an empty slot (task==NULL), also require IDLE status
             * to skip slots that are being set up by another concurrent pthread_create */
            if (task == NULL && threads[i].status != THREAD_STATE_IDLE)
                continue;
            break;
        }
    }

    return i;
}

BOOL
OpenTimerDevice(struct IORequest *io, struct MsgPort *mp, struct Task *task) {
    BYTE signal;

    // prepare MsgPort
    mp->mp_Node.ln_Type = NT_MSGPORT;
    mp->mp_Node.ln_Pri = 0;
    mp->mp_Node.ln_Name = NULL;
    mp->mp_Flags = PA_SIGNAL;
    mp->mp_SigTask = task;
    signal = AllocSignal(-1);
    if (signal == -1) {
        signal = SIGB_TIMER_FALLBACK;
        SetSignal(0, SIGF_TIMER_FALLBACK);
    }
    mp->mp_SigBit = signal;
    NewList(&mp->mp_MsgList);

    // prepare IORequest
    io->io_Message.mn_Node.ln_Type = NT_MESSAGE;
    io->io_Message.mn_Node.ln_Pri = 0;
    io->io_Message.mn_Node.ln_Name = NULL;
    io->io_Message.mn_ReplyPort = mp;
    io->io_Message.mn_Length = sizeof(struct TimeRequest);

    // open timer.device
    return !OpenDevice((STRPTR) TIMERNAME, UNIT_MICROHZ, io, 0);
}

void
CloseTimerDevice(struct IORequest *io) {
    struct MsgPort *mp;

    if (!CheckIO(io)) {
        AbortIO(io);
    }
    WaitIO(io);

    if (io->io_Device != NULL)
        CloseDevice(io);

    mp = io->io_Message.mn_ReplyPort;
    if (mp->mp_SigBit != SIGB_TIMER_FALLBACK)
        FreeSignal(mp->mp_SigBit);
}