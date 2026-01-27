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

    MutexObtain(thread_sem);

    /* Check if thread already terminated */
    if (inf->status == THREAD_STATE_DESTRUCT || inf->status == THREAD_STATE_TERMINATED) {
        if (value_ptr)
            *value_ptr = inf->ret;

        _pthread_clear_threadinfo(inf);
        MutexRelease(thread_sem);
        return 0;
    }

    /* Thread still running - allocate signal and register as joiner */
    /* Only if we are a pthread thread (me != NULL) */
    if (me) {
        me->join_signal = AllocSignal(-1);
        if (me->join_signal == -1)
            me->join_signal_mask = SIGF_PARENT;
        else
            me->join_signal_mask = 1L << me->join_signal;

        me->join_thread_id = thread;
    }

    MutexRelease(thread_sem);

    /* Wait for thread to finish */
    if (me) {
        /* We are a pthread - wait for signal */
        /* CRITICAL: Check one more time if thread already exited to prevent lost signal */
        if (inf->status != THREAD_STATE_DESTRUCT && inf->status != THREAD_STATE_TERMINATED) {
            uint32_t sigs = Wait(me->join_signal_mask | me->cancel_signal_mask);

            if (sigs & me->cancel_signal_mask)
                pthread_testcancel();
        }
        /* If thread already DESTRUCT/TERMINATED, skip Wait - it already signaled or exited */
    } else {
        /* We are main thread or non-pthread - busy wait with proper locking */
        int done = 0;
        while (!done) {
            Delay(1); /* Wait 1 tick (~20ms) */
            MutexObtain(thread_sem);
            if (inf->status == THREAD_STATE_DESTRUCT || inf->status == THREAD_STATE_TERMINATED)
                done = 1;
            MutexRelease(thread_sem);
        }
    }

    MutexObtain(thread_sem);

    /* Get return value and clean up */
    if (value_ptr)
        *value_ptr = inf->ret;

    if (me) {
        me->join_thread_id = 0;
        if (me->join_signal != -1) {
            FreeSignal(me->join_signal);
            me->join_signal = -1;
            me->join_signal_mask = 0;
        }
    }

    _pthread_clear_threadinfo(inf);

    MutexRelease(thread_sem);
    return 0;
}
