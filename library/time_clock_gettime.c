/*
 * $Id: time_gettime.c,v 1.1 2020-01-31 16:55:42 apalmate Exp $
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

#define __USE_OLD_TIMEVAL__

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#ifndef _TIMEZONE_HEADERS_H
#include "timezone_headers.h"
#endif /* _TIMEZONE_HEADERS_H */

extern struct TimerIFace *NOCOMMON __ITimer;
extern BOOL NOCOMMON __timer_busy;

int clock_gettime(clockid_t clk_id, struct timespec *t)
{
    ENTER();
    
    /* Check the supported flags.  */
    if ((clk_id & ~(CLOCK_MONOTONIC | CLOCK_REALTIME)) != 0)
    {
        __set_errno(EINVAL);
        RETURN(-1);
        return -1;
    }

    if (__timer_busy)
    {
        __set_errno(EAGAIN);
        RETURN(-1);
        return -1;
    }

    DECLARE_TIMEZONEBASE();
    struct TimerIFace *ITimer = __ITimer;

    struct timeval tv;
    int result = 0;
    uint32 gmtoffset = 0;
    int8 dstime = -1;

    //Set default value for tv
    tv.tv_secs = tv.tv_micro = 0;

    GetTimezoneAttrs(NULL, TZA_UTCOffset, &gmtoffset, TZA_TimeFlag, &dstime, TAG_DONE);
    if (result == 0)
    {
        __timer_busy = TRUE;
        if (clk_id == CLOCK_MONOTONIC)
        {
            /*
            CLOCK_MONOTONIC
                A nonsettable system-wide clock that represents monotonic
                time since—as described by POSIX—"some unspecified point
                in the past". On clib2, that point corresponds to the
                number of seconds that the system has been running since
                it was booted.
            */
            GetUpTime((struct TimeVal *)&tv);
        }
        else
        {
            /*
            A settable system-wide clock that measures real (i.e.,
                wall-clock) time.  Setting this clock requires appropriate
                privileges.  This clock is affected by discontinuous jumps
                in the system time (e.g., if the system administrator
                manually changes the clock), and by the incremental
                adjustments performed by adjtime(3) and NTP.
            */
            GetSysTime((struct TimeVal *)&tv);
        }

        if (result == 0)
        {
            if (clk_id == CLOCK_MONOTONIC)
            {
                t->tv_sec = tv.tv_sec;
                t->tv_nsec = tv.tv_micro * 1000;
            }
            else
            {
                /* 2922 is the number of days between 1.1.1970 and 1.1.1978 */
                tv.tv_sec += (2922 * 24 * 60 + gmtoffset) * 60;
                t->tv_sec = tv.tv_sec;
                t->tv_nsec = tv.tv_micro * 1000;
            }
        }
    }

    __timer_busy = FALSE;
    RETURN(result);
    return result;
}
