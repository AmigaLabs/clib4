/*
 * $Id: time_settimeofday.c,v 1.0 2022-02-24 18:31:57 apalmate Exp $
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

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

extern struct MsgPort     NOCOMMON *__timer_port;
extern struct TimeRequest NOCOMMON *__timer_request;
extern BOOL NOCOMMON __timer_busy;

int settimeofday (const struct timeval *t, const struct timezone *tz) {

    ENTER();

    int result = -1;
    int32 __gmtoffset = 0;

    if (__timer_busy)
    {
        __set_errno(EPERM);
        RETURN(result);
        return result;
    }

    __timer_busy = TRUE;

    __timer_request->Request.io_Message.mn_ReplyPort = __timer_port;
    __timer_request->Request.io_Command = TR_SETSYSTIME;
    if (tz != NULL) {
        __gmtoffset = tz->tz_minuteswest;
    }
    /* 2922 is the number of days between 1.1.1970 and 1.1.1978 */
    __timer_request->Time.Seconds = t->tv_secs - ((2922 * 24 * 60 + __gmtoffset) * 60);
    __timer_request->Time.Microseconds = t->tv_micro;

    DoIO((struct IORequest *)__timer_request);
    GetMsg(__timer_port);

    result = 0;
    __set_errno(0);

    __timer_busy = FALSE;

    RETURN(result);
    return result;
}