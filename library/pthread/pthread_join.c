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
    struct Task *task = FindTask(NULL);

    if (inf == NULL || inf->parent == NULL)
        return ESRCH;

    if (inf->detached)
        return EINVAL;

    if ((struct Task *) inf->task == task) {
        return EDEADLK;
    }

	pthread_testcancel();

    /* Wait for the thread to reach THREAD_STATE_DESTRUCT state
     * The thread sets this state just before it exits in StarterFunc()
     * and signals the parent with the thread-specific signal
     */
    D(("pthread_join: thread %ld has signal bit %d\n", thread, inf->parent_signal));

    /* Calculate the signal mask for this specific thread */
    uint32_t thread_signal = (inf->parent_signal != -1) ? (1L << inf->parent_signal) : 0;

    if (thread_signal == 0) {
        D(("pthread_join: ERROR - thread %ld has no valid signal!\n", thread));
        return EINVAL;
    }

    while (inf->status != THREAD_STATE_DESTRUCT) {
        /* Wait for the thread to signal us
         * Note: If the thread already reached THREAD_STATE_DESTRUCT before we get here,
         * we skip the wait (which is correct - thread is already done)
         */
        D(("pthread_join: waiting for thread %ld to terminate - current status %ld - waiting on signal bit %d\n", thread, inf->status, inf->parent_signal));
        uint32_t sigs = Wait(thread_signal | SIGBREAKF_CTRL_C);
        D(("pthread_join: woke up from wait for thread %ld - thread_signal %ld - SIGBREAKF_CTRL_C %ld\n", thread, sigs & thread_signal, sigs & SIGBREAKF_CTRL_C));
        /* Check if we got interrupted */
        if (sigs & SIGBREAKF_CTRL_C) {
            D(("pthread_join: woke up from wait for thread %ld - current status after pthread_testcancel %ld\n", thread, inf->status));
            pthread_testcancel();
        }
    }

    /* Get the return value before cleanup */
    if (value_ptr)
        *value_ptr = inf->ret;

    /* Always clean up the thread info, even if thread exited very quickly
     * This fixes the race condition where thread exits before pthread_join is called
     */
    MutexObtain(thread_sem);
    _pthread_clear_threadinfo(inf);
    MutexRelease(thread_sem);

    return 0;
}
