/*
  $Id: pthread_mutex_timedlock.c,v 1.00 2022-07-18 12:09:49 clib4devs Exp $

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

// Helper function to subtract two timespecs (ts1 = ts2 - ts3)
static void timespec_sub(struct timespec *ts1, const struct timespec *ts2, const struct timespec *ts3) {
    ts1->tv_sec = ts2->tv_sec - ts3->tv_sec;
    if (ts2->tv_nsec < ts3->tv_nsec) {
        ts1->tv_sec--;
        ts1->tv_nsec = 1000000000L + ts2->tv_nsec - ts3->tv_nsec;
    } else {
        ts1->tv_nsec = ts2->tv_nsec - ts3->tv_nsec;
    }
}

int
pthread_mutex_timedlock(pthread_mutex_t *mutex, const struct timespec *abstime) {
    int result;
    struct _clib4 *__clib4 = __CLIB4;

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
		else if (mutex->kind != PTHREAD_MUTEX_RECURSIVE && MutexIsMine(mutex))
			return EDEADLK;
	} else {
		// Successfully acquired the lock on first try
		return 0;
	}

    // abstime is absolute time, need to convert to relative for timer.device
    struct timespec starttime;
    struct timespec timeout;

    clock_gettime(CLOCK_REALTIME, &starttime);
    timespec_sub(&timeout, abstime, &starttime);

    // Check if timeout is already in the past
    if (timeout.tv_sec < 0 || (timeout.tv_sec == 0 && timeout.tv_nsec <= 0)) {
        return ETIMEDOUT;
    }

    uint32 sigMask = 1L << timedTimerPort->mp_SigBit;

    timedTimerIO->Request.io_Command = TR_ADDREQUEST;
    timedTimerIO->Request.io_Flags = 0;
    TIMESPEC_TO_OLD_TIMEVAL(&timedTimerIO->Time, &timeout);

    SetSignal(0, sigMask);
    SendIO((struct IORequest *) timedTimerIO);

    result = MutexAttemptWithSignal(mutex->mutex, sigMask);

    // Abort timer if we got the lock
    if (!(result & sigMask)) {
        AbortIO((struct IORequest *) timedTimerIO);
        WaitIO((struct IORequest *) timedTimerIO);
    }

    if (result & sigMask)
        result = ETIMEDOUT;
    else
        result = 0;

    return result;
}