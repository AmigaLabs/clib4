/*
  $Id: pthread_getspecific.c,v 1.00 2022-07-18 12:09:49 clib4devs Exp $

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

void *
pthread_getspecific(pthread_key_t key) {
    ThreadInfo *inf;
    BOOL key_is_valid;
    void *value = NULL;

    /* NOTE: D() macros disabled in this function to avoid potential deadlock
     * if debug output somehow calls pthread_getspecific/setspecific while
     * holding tls_sem mutex */

    if (key >= PTHREAD_KEYS_MAX || key < 0)
        return NULL;

    /* Get current thread info */
    inf = GetCurrentThreadInfo();
    if (inf == NULL)
        return NULL;

    /* Use global lock to protect the entire operation */
    /* This prevents race with pthread_key_delete */
    MutexObtain(tls_sem);

    /* Check if key is valid INSIDE the lock */
    if (tlskeys[key].used) {
        /* Read from tlsvalues INSIDE the lock to prevent race with key_delete */
        value = inf->tlsvalues[key];
    }

    MutexRelease(tls_sem);

    return value;
}
