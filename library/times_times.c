/*
 * $Id: times_times.c,v 1.0 2021-02-07 12:42:45 apalmate Exp $
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

clock_t
times(struct tms *tp)
{
    struct rusage ru;
    time_t tim;

    if (getrusage(RUSAGE_SELF, &ru) < 0)
        return (clock_t)-1;

    tp->tms_utime = (clock_t)ru.ru_utime.tv_sec * CLK_TCK + (ru.ru_utime.tv_usec * CLK_TCK) / 1000000;
    tp->tms_stime = (clock_t)ru.ru_stime.tv_sec * CLK_TCK + (ru.ru_stime.tv_usec * CLK_TCK) / 1000000;

    if (getrusage(RUSAGE_CHILDREN, &ru) < 0)
        return (clock_t)-1;

    tp->tms_cutime = 0;
    tp->tms_cstime = 0;

    if ((tim = time(NULL)) == (time_t)-1)
        return (clock_t)-1;

    /* The value we are supposed to return does not fit in 32 bits.
      Still, it is useful if you are interested in time differences
      in CLK_TCKths of a second.
   */

    return (clock_t)tim * CLK_TCK;
}