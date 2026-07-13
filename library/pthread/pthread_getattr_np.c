/*
  $Id: pthread_getattr_np.c,v 1.00 2022-07-18 12:09:49 clib4devs Exp $

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
pthread_getattr_np(pthread_t thread, pthread_attr_t *attr) {
    ThreadInfo *inf;

    if (attr == NULL)
        return EINVAL;

    inf = GetThreadInfo(thread);

    if (inf == NULL)
        return ESRCH; // TODO

    *attr = inf->attr;

    /* Report the thread's ACTUAL stack, matching the Linux/glibc
     * semantics of pthread_getattr_np: callers chain this into
     * pthread_attr_getstack() to derive the live stack bounds (stack
     * overflow detection, GC stack scanning, ...).  The creation-time
     * attr normally has stackaddr == NULL (and stacksize == 0 when the
     * default was used), which sends such callers computing bounds far
     * off the real stack.  The exec Task always knows the truth. */
    if (inf->task != NULL) {
        struct Task *t = (struct Task *) inf->task;
        attr->stackaddr = t->tc_SPLower;
        attr->stacksize = (size_t) ((char *) t->tc_SPUpper - (char *) t->tc_SPLower);
    }

    return 0;
}