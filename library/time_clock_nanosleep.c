/*
 * $Id: time_clock_nanosleep.c,v 1.0 2021-03-01 17:55:42 apalmate Exp $
 *
 * :ts=4
 *
 * Portable ISO 'C' (1994) runtime library for the Amiga computer
 * Copyright (c) 2002-2015 by Olaf Barthel <obarthel (at) gmx.net>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   - Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *
 *   - Neither the name of Olaf Barthel nor the names of contributors
 *     may be used to endorse or promote products derived from this
 *     software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _TIME_HEADERS_H
#include "time_headers.h"
#endif /* _TIME_HEADERS_H */

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

int clock_nanosleep(clockid_t clock_id, int flags, const struct timespec *request, struct timespec *remain) {
    ENTER();
    
    // Validate timespec
    if (request == NULL || request->tv_nsec < 0 || (unsigned long)request->tv_nsec >= NSEC_PER_SEC) {
        __set_errno(EINVAL);
        RETURN(-1);
        return -1;
    }

    // Validate clock
    switch (clock_id) {
        case CLOCK_MONOTONIC:
        case CLOCK_REALTIME:
            break;
        default:
        __set_errno(EINVAL);
            RETURN(-1);
            return -1;
    }

    if (flags == TIMER_ABSTIME) {
        struct timespec ts_rel;
        struct timespec ts_now;

        do {
            // Get current time with requested clock
            if (clock_gettime(clock_id, &ts_now) != 0) {
                RETURN(-1);
                return -1;
            }

            // Calculate relative timespec
            ts_rel.tv_sec  = request->tv_sec  - ts_now.tv_sec;
            ts_rel.tv_nsec = request->tv_nsec - ts_now.tv_nsec;
            if (ts_rel.tv_nsec < 0) {
                ts_rel.tv_sec  -= 1;
                ts_rel.tv_nsec += NSEC_PER_SEC;
            }

            // Check if time already elapsed
            if (ts_rel.tv_sec == 0 && ts_rel.tv_nsec == 0) {
                RETURN(0);
                return 0;
            }

            // "The absolute clock_nanosleep() function has no effect on the
            // structure referenced by remain", so do not pass remain here
        } while (nanosleep(&ts_rel, NULL) == 0);

        // If nanosleep failed or was interrupted by a signal,
        // return so the caller can handle it appropriately
        RETURN(-1);
        return -1;
    } else if (flags == 0) {
        int result = nanosleep(request, remain);
        RETURN(result);
        return result;
    } else {
        // Invalid flags
        __set_errno(EINVAL);
        RETURN(-1);
        return -1;
    }
}