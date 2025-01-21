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

    if (inf == NULL || inf->parent == NULL || inf->canceled == TRUE)
        return ESRCH;

    inf->canceled = TRUE;

    // we might have to cancel the thread immediately
    if (inf->canceltype == PTHREAD_CANCEL_ASYNCHRONOUS && inf->cancelstate == PTHREAD_CANCEL_ENABLE) {
        struct Task *task = FindTask(NULL);

        if ((struct Task *) inf->task == task)
            pthread_testcancel(); // cancel ourselves
        else
            Signal((struct Task *) inf->task, SIGBREAKF_CTRL_C); // trigger the exception handler
    } else {
        // for the timed waits
        Signal((struct Task *) inf->task, SIGBREAKF_CTRL_C);
    }

    return 0;
}