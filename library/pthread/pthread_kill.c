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

    /* Validate signal number; sig == 0 is special (existence check) */
    if (sig < 0 || sig >= NSIG)
        return EINVAL;

    /* Look up the target thread */
    inf = GetThreadInfo(thread);
    if (inf == NULL || inf->task == NULL)
        return ESRCH;

    /* sig == 0: just check that the thread exists */
    if (sig == 0)
        return 0;

    /*
     * AmigaOS4 does not have memory protection and cannot safely deliver
     * POSIX signals to another task's context.  For terminal signals we
     * map to SIGBREAKF_CTRL_C, which is the conventional AmigaOS way to
     * ask a task to abort.  For other signals we attempt a best-effort
     * delivery by marking the signal as pending in the target thread's
     * clib4 context and waking it up so that it can process the signal
     * at a safe point.
     */

    /* Terminal signals: map to the AmigaOS CTRL-C break signal */
    if (sig == SIGTERM || sig == SIGKILL || sig == SIGINT || sig == SIGQUIT) {
        Signal((struct Task *) inf->task, SIGBREAKF_CTRL_C);
        return 0;
    }

    /* For all other signals, access the target thread's clib4 context */
    target_clib4 = (struct _clib4 *) inf->task->pr_UID;
    if (target_clib4 == NULL)
        return EINVAL;

    /*
     * If the signal is blocked in the target thread, mark it as pending.
     * __signals_blocked is the persistent mask set by sigprocmask/sigblock,
     * while local_signals_blocked is a temporary per-signal block used
     * during handler execution to prevent recursive delivery.
     */
    if (FLAG_IS_SET(target_clib4->__signals_blocked, sigmask(sig)) ||
        FLAG_IS_SET(target_clib4->local_signals_blocked, sigmask(sig))) {
        SET_FLAG(target_clib4->local_raised_signals_blocked, sigmask(sig));
        return 0;
    }

    /* Signal is not blocked: check whether the target has a custom handler.
     * The table is indexed as sig - SIGHUP; the top-level check (sig < NSIG)
     * and the SIGHUP lower-bound guard together guarantee that the index
     * stays within the __signal_handler_table[NSIG] array bounds. */
    if (sig >= SIGHUP && sig < NSIG) {
        signal_handler_t handler = target_clib4->__signal_handler_table[sig - SIGHUP];

        if (handler == SIG_IGN)
            return 0;

        if (handler != SIG_DFL && handler != NULL) {
            /*
             * A custom handler is installed.  We cannot call it in the
             * target thread's context on AmigaOS4, so we mark the signal
             * as pending and wake the thread via SIGBREAKF_CTRL_C so that
             * it will process the pending signal at its next cancellation
             * point or signal-check opportunity.
             */
            SET_FLAG(target_clib4->local_raised_signals_blocked, sigmask(sig));
            Signal((struct Task *) inf->task, SIGBREAKF_CTRL_C);
            return 0;
        }
    }

    return EINVAL;
}