/*
  $Id: pthread_mutex_lock.c,v 1.00 2022-07-18 12:09:49 clib4devs Exp $

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
pthread_mutex_lock(pthread_mutex_t *mutex) {
    ENTER();
    SHOWPOINTER(mutex);

    if (!mutex) {
        LEAVE();
        return EINVAL;
    }

    if (mutex->mutex == NULL) {
        SHOWMSG("mutex was not initalized. Initialize it");
        int ret = _pthread_mutex_init(mutex, NULL, TRUE);
        if (ret != 0) {
            SHOWMSG("Cannot initialize mutex");
            LEAVE();
            return EINVAL;
        }
    }

    // normal mutexes would simply deadlock here
    if (mutex->kind == PTHREAD_MUTEX_ERRORCHECK) {
        SHOWMSG("MutexAttempt");
        BOOL isLocked = MutexAttempt(mutex->mutex);
        if (!isLocked) {
            SHOWMSG("DeadLock");
            LEAVE();
            return EDEADLK;
        } else {
            MutexRelease(mutex->mutex);
        }
    }

    SHOWMSG("MutexObtain");
    MutexObtain(mutex->mutex);
    SHOWMSG("Done");

    RETURN(0);
    return 0;
}
