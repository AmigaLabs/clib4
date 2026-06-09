/*
  $Id: pthread_cancel.c,v 1.00 2022-07-18 12:09:49 clib4devs Exp $

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
pthread_cancel(pthread_t thread) {
    ThreadInfo *inf;

    inf = GetThreadInfo(thread);

    if (inf == NULL)
        return ESRCH;


    /* Hold thread_sem while reading/writing ThreadInfo fields.
     * Without this lock, the target thread could exit between our
     * validation checks and the Signal() call, leaving inf->task NULL
     * which would crash Signal(NULL, mask). */
    MutexObtain(thread_sem);

    if (inf->task == NULL || inf->status == THREAD_STATE_IDLE) {
        MutexRelease(thread_sem);
        return ESRCH;
    }

    if (inf->canceled == TRUE) {
        MutexRelease(thread_sem);
        return ESRCH;
    }

    inf->canceled = TRUE;

    /* Check if we are cancelling ourselves */
    struct Task *self = FindTask(NULL);
    if ((struct Task *)inf->task == self) {
        MutexRelease(thread_sem);
        /* Cancel ourselves — must call testcancel outside the lock
         * because it may call pthread_exit which longjmps. */
        if (inf->canceltype == PTHREAD_CANCEL_ASYNCHRONOUS &&
            inf->cancelstate == PTHREAD_CANCEL_ENABLE)
            pthread_testcancel();
        return 0;
    }

    /* Signal the target thread's cancel signal to wake it from any
     * blocking wait (timed or otherwise). */
    if (inf->cancel_signal_mask != 0)
        Signal((struct Task *)inf->task, inf->cancel_signal_mask);

    MutexRelease(thread_sem);


    return 0;
}