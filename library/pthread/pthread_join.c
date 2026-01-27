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
    D(("pthread_join: ENTER thread=%ld\n", thread));

    ThreadInfo *inf = GetThreadInfo(thread);

    if (inf == NULL) {
        D(("pthread_join: inf is NULL, returning ESRCH\n"));
        return ESRCH;
    }

    if (inf->parent == NULL) {
        D(("pthread_join: parent is NULL, returning ESRCH\n"));
        return ESRCH;
    }

    ThreadInfo *me = GetCurrentThreadInfo();
    struct Task *task = FindTask(NULL);

    D(("pthread_join: checks passed for thread %ld\n", thread));

    if (inf->detached) {
        D(("pthread_join: thread %ld is detached, returning EINVAL\n", thread));
        return EINVAL;
    }

    if ((struct Task *) inf->task == task) {
        D(("pthread_join: deadlock detected, returning EDEADLK\n"));
        return EDEADLK;
    }

    D(("pthread_join: thread %ld proceeding to join\n", thread));

    MutexObtain(thread_sem);

    D(("pthread_join: acquired lock, status=%d\n", inf->status));

    /* Check if thread already terminated */
    if (inf->status == THREAD_STATE_DESTRUCT || inf->status == THREAD_STATE_TERMINATED) {
        D(("pthread_join: thread %ld already terminated, status=%d\n", thread, inf->status));

        if (value_ptr) {
            D(("pthread_join: setting return value\n"));
            *value_ptr = inf->ret;
        }

        /* Thread process has already exited - just clean up ThreadInfo */
        D(("pthread_join: calling _pthread_clear_threadinfo for thread %ld\n", thread));
        _pthread_clear_threadinfo(inf);
        D(("pthread_join: _pthread_clear_threadinfo returned\n"));

        MutexRelease(thread_sem);
        D(("pthread_join: released lock, returning 0\n"));
        return 0;
    }

    D(("pthread_join: thread %ld still running (status=%d), need to wait\n", thread, inf->status));

    /* Thread still running - allocate signal and register as joiner BEFORE releasing lock */
    if (me) {
        me->join_signal = AllocSignal(-1);
        if (me->join_signal == -1) {
            me->join_signal_mask = SIGF_PARENT;
            D(("pthread_join: using SIGF_PARENT fallback\n"));
        } else {
            me->join_signal_mask = 1L << me->join_signal;
            D(("pthread_join: allocated join signal %d\n", me->join_signal));
        }
        /* Register as joiner BEFORE releasing the lock so thread can find us */
        me->join_thread_id = thread;
        D(("pthread_join: registered as joiner for thread %ld (join_thread_id set BEFORE releasing lock)\n", thread));
    }

    /* NOW release the lock - thread can find us in its search */
    MutexRelease(thread_sem);

    /* Wait for the thread to signal us */
    D(("pthread_join: waiting for thread %ld\n", thread));
    uint32_t sigs = Wait(me->join_signal_mask | me->cancel_signal_mask);
    D(("pthread_join: woke up from wait\n"));

    if (sigs & me->cancel_signal_mask) {
        pthread_testcancel();
    }

    MutexObtain(thread_sem);

    /* Thread has already exited and signaled us - just clean up */
    D(("pthread_join: thread %ld has signaled, cleaning up\n", thread));

    /* Get return value and clean up */
    if (value_ptr)
        *value_ptr = inf->ret;

    me->join_thread_id = 0;
    if (me->join_signal != -1) {
        FreeSignal(me->join_signal);
        me->join_signal = -1;
        me->join_signal_mask = 0;
    }

    _pthread_clear_threadinfo(inf);

    MutexRelease(thread_sem);
    return 0;
}
