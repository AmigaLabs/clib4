/*
  $Id: pthread_kill.c,v 1.00 2022-07-18 12:09:49 clib4devs Exp $

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
pthread_kill(pthread_t thread, int sig) {
    ThreadInfo *inf;
    struct _clib4 *target_clib4;

    /* Validate signal number first; sig == 0 is special (existence check) */
    if (sig < 0 || sig >= NSIG)
        return EINVAL;

    inf = GetThreadInfo(thread);

    /* Validate thread exists and is active */
    if (inf == NULL || inf->task == NULL)
        return ESRCH;

    if (inf->status == THREAD_STATE_IDLE ||
        inf->status == THREAD_STATE_DESTRUCT ||
        inf->status == THREAD_STATE_TERMINATED ||
        inf->status == THREAD_STATE_TERMINATING)
        return ESRCH;

    /* sig == 0: just check if thread exists (POSIX) */
    if (sig == 0)
        return 0;

    /* Access the target thread's clib4 context (stored in pr_UID) */
    target_clib4 = (struct _clib4 *) inf->task->pr_UID;
    if (target_clib4 == NULL)
        return EINVAL;

    /*
     * If the signal is blocked in the target, mark it as pending and return.
     */
    if (FLAG_IS_SET(target_clib4->__signals_blocked, sigmask(sig)) ||
        FLAG_IS_SET(target_clib4->local_signals_blocked, sigmask(sig))) {
        SET_FLAG(target_clib4->local_raised_signals_blocked, sigmask(sig));
        return 0;
    }

    /* Check handler disposition */
    if (sig >= SIGHUP && sig < NSIG) {
        signal_handler_t handler = target_clib4->__signal_handler_table[sig - SIGHUP];
        if (handler == SIG_IGN)
            return 0;
    }

    /*
     * Mark the signal as pending and send SIGBREAKF_CTRL_C to interrupt
     * the target thread.
     *
     * If the thread has installed the task exception hook (via StarterFunc),
     * AmigaOS4 fires it even while the thread is executing non-blocking code
     * (e.g. a tight loop), giving us asynchronous-style signal delivery.
     * The hook dispatches the signal: calling custom handlers directly, or
     * calling pthread_exit() for terminal signals (which uses longjmp() to
     * unwind cleanly back to the setjmp() point in StarterFunc).
     *
     * If the thread is blocked in Wait() or a similar function,
     * SIGBREAKF_CTRL_C simply wakes it and the signal is processed at the
     * next pthread_testcancel() cancellation point.
     */
    SET_FLAG(target_clib4->local_raised_signals_blocked, sigmask(sig));
    Signal((struct Task *) inf->task, SIGBREAKF_CTRL_C);
    return 0;
}