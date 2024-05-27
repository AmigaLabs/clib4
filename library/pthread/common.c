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

pthread_t GetThreadId(struct Task *task) {
    pthread_t i;

    // 0 is main task, First thread id will be 1 so that it is different than default value of pthread_t
    for (i = PTHREAD_FIRST_THREAD_ID; i < PTHREAD_THREADS_MAX; i++) {
        if (threads[i].task == (struct Process *) task)
            break;
    }

    return i;
}

struct timerequest *
OpenTimerDevice() {
    BYTE signal = AllocSignal(-1);
    struct TimeRequest *io;
    struct MsgPort *mp;

    if (signal == -1) {
        signal = SIGB_TIMER_FALLBACK;
        SetSignal(SIGF_TIMER_FALLBACK, 0);
    }
    // Allocate MsgPort
    mp = AllocSysObjectTags(ASOT_PORT,
                        ASOPORT_AllocSig, FALSE,
                        ASOPORT_Signal, signal);
    if (!mp)
        return TRUE;

    // allocate IORequest
    io = AllocSysObjectTags(ASOT_IOREQUEST,
                             ASOIOR_ReplyPort, mp,
                             ASOIOR_Size, sizeof(struct TimeRequest),
                             TAG_END);
    if (!io) {
        FreeSysObject(ASOT_PORT, mp);
        mp = NULL;
        return TRUE;
    }

    // open timer.device
    if (OpenDevice((STRPTR) TIMERNAME, UNIT_MICROHZ, io, 0)) {
        if (mp->mp_SigBit != SIGB_TIMER_FALLBACK)
            FreeSignal(mp->mp_SigBit);
        FreeSysObject(ASOT_PORT, mp);
        FreeSysObject(ASOT_IOREQUEST, io);
        io = NULL;
        mp = NULL;
        return NULL;
    }
    return (struct timerequest *) io;
}

void
CloseTimerDevice(struct IORequest *io) {
    struct MsgPort *mp;

    if (!CheckIO(io)) {
        AbortIO(io);
        WaitIO(io);
    }

    if (io->io_Device != NULL)
        CloseDevice(io);

    mp = io->io_Message.mn_ReplyPort;

    if (mp->mp_SigBit != SIGB_TIMER_FALLBACK)
        FreeSignal(mp->mp_SigBit);

    if (mp) {
        FreeSysObject(ASOT_PORT, mp);
        mp = NULL;
    }
    if (io) {
        FreeSysObject(ASOT_IOREQUEST, io);
        io = NULL;
    }
}