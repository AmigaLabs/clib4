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
    ThreadInfo *inf = GetThreadInfo(thread);

    if (inf == NULL)
        return ESRCH;

    /* Validate signal number */
    if (sig < 0 || sig >= NSIG)
        return EINVAL;

    /* Hold thread_sem to prevent inf->task from being cleared by concurrent exit */
    MutexObtain(thread_sem);

    /* Validate thread is active */
    if (inf->task == NULL ||
        inf->status == THREAD_STATE_IDLE ||
        inf->status == THREAD_STATE_DESTRUCT ||
        inf->status == THREAD_STATE_TERMINATED) {
        MutexRelease(thread_sem);
        return ESRCH;
    }

    /* sig == 0: just check if thread exists (POSIX) */
    if (sig == 0) {
        MutexRelease(thread_sem);
        return 0;
    }

    /* Use the thread's cancel signal for cancellation (SIGCANCEL-like),
     * otherwise use SIGBREAKF_CTRL_C as a generic interrupt.
     * Direct POSIX-to-AmigaOS signal bit mapping is not meaningful
     * since AmigaOS signal bits have per-task allocation semantics. */
    if (inf->cancel_signal_mask != 0)
        Signal((struct Task *)inf->task, inf->cancel_signal_mask);
    else
        Signal((struct Task *)inf->task, SIGBREAKF_CTRL_C);

    MutexRelease(thread_sem);

    return 0;
}