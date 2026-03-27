/*
  $Id: pthread_join.c,v 1.00 2022-07-18 12:09:49 clib4devs Exp $

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

int
pthread_join(pthread_t thread, void **value_ptr) {
    ThreadInfo *inf = GetThreadInfo(thread);

    if (inf == NULL)
        return ESRCH;

    if (inf->parent == NULL)
        return ESRCH;

    ThreadInfo *me = GetCurrentThreadInfo();
    struct Task *task = FindTask(NULL);

    if (inf->detached)
        return EINVAL;

    if ((struct Task *) inf->task == task)
        return EDEADLK;

    D(("pthread_join[%ld]: About to acquire thread_sem (first time)\n", thread));
    MutexObtain(thread_sem);
    D(("pthread_join[%ld]: Acquired thread_sem, status=%d\n", thread, inf->status));

    /* Check if thread already terminated */
    if (inf->status == THREAD_STATE_DESTRUCT || inf->status == THREAD_STATE_TERMINATED) {
        D(("pthread_join[%ld]: Thread already terminated, cleaning up\n", thread));
        if (value_ptr)
            *value_ptr = inf->ret;

        _pthread_clear_threadinfo(inf);
        MutexRelease(thread_sem);
        return 0;
    }

    /* Thread still running - register as joiner */
    /* Only if we are a pthread thread (me != NULL) */
    if (me) {
        /* Add self to joiners list */
        me->status = THREAD_STATE_JOINING;
        me->join_thread_id = thread; /* I am waiting for 'thread' */
        me->join_result = NULL;
        AddHead((struct List *)&join_list, (struct Node *)&me->join_node);
        D(("pthread_join[%ld]: Added self (id=%d) to join_list\n", thread, me->thread_id));
    }

    D(("pthread_join[%ld]: Releasing thread_sem, waiting for signal\n", thread));
    MutexRelease(thread_sem);

    /* Wait for thread to finish */
    if (me) {
        for (;;) {
            /* Check status under lock BEFORE calling Wait().
             * This closes the race window where the target finishes and signals
             * us between the MutexRelease above (or a loop iteration) and Wait().
             * Without this check, the signal could be lost, leaving us stuck
             * in Wait() forever.
             *
             * NOTE: We do NOT call SetSignal(0, join_signal_mask) here.
             * Doing so would destroy signals sent by other threads that already
             * finished (e.g. threads 1..4 all signaling while we are joining
             * thread 0), causing the subsequent joins to block forever waiting
             * for a signal that was already consumed and then wiped out. The
             * check-before-wait pattern below is sufficient to avoid spurious
             * blocks without discarding valid pending signals. */
            MutexObtain(thread_sem);
            D(("pthread_join[%ld]: checking status=%d\n", thread, inf->status));

            if (inf->status == THREAD_STATE_DESTRUCT || inf->status == THREAD_STATE_TERMINATED) {
                break; /* Target confirmed done, we hold thread_sem for cleanup */
            }
            MutexRelease(thread_sem);

            /* Target still running - block until signaled */
            D(("pthread_join[%ld]: Going to Wait() for signal mask=0x%lx\n", thread, (unsigned long)me->join_signal_mask));
            uint32_t sigs = Wait(me->join_signal_mask | me->cancel_signal_mask);
            D(("pthread_join[%ld]: Wait() returned, sigs=0x%lx\n", thread, (unsigned long)sigs));

            if (sigs & me->cancel_signal_mask)
                pthread_testcancel();
        }

        /* We hold thread_sem and target is confirmed DESTRUCT/TERMINATED */
        Remove((struct Node *)&me->join_node);
        me->status = THREAD_STATE_RUNNING;

        if (value_ptr)
            *value_ptr = inf->ret;

        _pthread_clear_threadinfo(inf);

        MutexRelease(thread_sem);
        return 0;

    } else {
        /* GetCurrentThreadInfo() returned NULL - caller is not a recognized
         * pthread thread (main thread is threads[0] and always valid).
         * Cannot join without a valid caller context. */
        return ESRCH;
    }
}
