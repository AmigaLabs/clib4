/*
  $Id: pthread_mutex_timedlock.c,v 1.00 2022-07-18 12:09:49 clib2devs Exp $

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
pthread_mutex_timedlock(pthread_mutex_t *mutex, const struct timespec *abstime) {
    int result;
    struct TimeRequest timerio;
    struct Task *task = FindTask(NULL);
    struct MsgPort msgport;

    if (mutex == NULL)
        return EINVAL;

    if (abstime == NULL)
        return pthread_mutex_lock(mutex);
    else if (abstime->tv_nsec < 0 || abstime->tv_nsec >= 1000000000)
        return EINVAL;

    result = pthread_mutex_trylock(mutex);
    if (result != 0) {
        // pthread_mutex_trylock returns EBUSY when a deadlock would occur
        if (result != EBUSY)
            return result;
        else if (!MutexAttempt(mutex->mutex))
            return EDEADLK;
    }

    timerio.Request.io_Command = TR_ADDREQUEST;
    timerio.Time.Seconds = abstime->tv_sec;
    timerio.Time.Microseconds = abstime->tv_nsec/1000;

    if (!OpenTimerDevice((struct IORequest *) &timerio, &msgport, task)) {
        CloseTimerDevice((struct IORequest *) &timerio);
        return EINVAL;
    }
    uint32 sigMask = 1L << msgport.mp_SigBit;
    result = MutexAttemptWithSignal(mutex->mutex, sigMask);

    CloseTimerDevice((struct IORequest *) &timerio);

    if (result != 0)
        return ETIMEDOUT;

    return 0;
    //return _pthread_obtain_sema_timed(&mutex->semaphore, abstime, SM_EXCLUSIVE);
}