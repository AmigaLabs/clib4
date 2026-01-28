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
        /* We are a pthread - wait for signal */
        D(("pthread_join[%ld]: Going to Wait() for signal mask=0x%lx\n", thread, (unsigned long)me->join_signal_mask));
        uint32_t sigs = Wait(me->join_signal_mask | me->cancel_signal_mask);
        D(("pthread_join[%ld]: Wait() returned, sigs=0x%lx\n", thread, (unsigned long)sigs));

        if (sigs & me->cancel_signal_mask)
            pthread_testcancel();

        /* Re-acquire lock to check result/cleanup */
        D(("pthread_join[%ld]: About to acquire thread_sem (second time)\n", thread));
        MutexObtain(thread_sem);
        D(("pthread_join[%ld]: Acquired thread_sem (second time)\n", thread));

        /* We just need to ensure we are not in list anymore (if cancelled or spurious) */
        /* But if signaled by target, we might still be in list? */
        /* Target thread in clib4 (modified below) will set join_result and signal. */

        /* Safe to remove if nodes are valid */
        Remove((struct Node *)&me->join_node);

        /* Restore state */
        me->status = THREAD_STATE_RUNNING; // or whatever

        if (value_ptr)
            *value_ptr = me->join_result;

        /* Verify target thread status? */
        /* If we were signaled, target thread should be DESTRUCT/TERMINATED. */
        /* Target thread cleans ITSELF up? No, joiner cleans target up usually. */

        _pthread_clear_threadinfo(inf);

        MutexRelease(thread_sem);
        return 0;

    } else {
        /* We are main thread or non-pthread - busy wait with proper locking */
        /* Main thread should also use signals ideally, but let's stick to existing loop for now or fix it? */
        /* Main thread has NO join_node? It has `me` if it calls `pthread_join`. */
        /* Wait, main thread corresponds to `threads[0]`. GetThreadInfo(0) returns &threads[0]. */
        /* So `me` IS valid for main thread. */
        /* If `pthreads_init` initialized `threads[0]`, then `me` is valid. */
        /* And main thread has `join_signal` allocated now. */
        /* So main thread CAN use the Wait() path! */
        /* Wait, `if (me)` check above handles it. */
        /* This else block is for... `if (me == NULL)`? */
        /* GetCurrentThreadInfo only returns NULL if TLS not set up or something. */

        int done = 0;
        while (!done) {
            Delay(1); /* Wait 1 tick (~20ms) */
            MutexObtain(thread_sem);
            if (inf->status == THREAD_STATE_DESTRUCT || inf->status == THREAD_STATE_TERMINATED)
                done = 1;
            MutexRelease(thread_sem);
        }
        MutexObtain(thread_sem);
        if (value_ptr)
            *value_ptr = inf->ret;
        _pthread_clear_threadinfo(inf);
        MutexRelease(thread_sem);
        return 0;
    }
}
