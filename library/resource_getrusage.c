/*
 * $Id: resource_getrusage.c,v 1.0 2021-01-19 16:35:27 apalmate Exp $
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
/****************************************************************************/

#ifndef _TIME_HEADERS_H
#include "time_headers.h"
#endif /* _TIME_HEADERS_H */

#ifndef _LOCALE_HEADERS_H
#include "locale_headers.h"
#endif /* _LOCALE_HEADERS_H */

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

#include <sys/resource.h>

int getrusage(int who, struct rusage *rusage)
{
    if (rusage == NULL) {
        __set_errno(EINVAL);
        return -1;
    }

    struct TimeVal clock;
    int status = 0;
    struct TimerIFace *ITimer = __ITimer;
    if (__global_clib2 == NULL)  {
        __set_errno(EINVAL);
        return -1;
    }

    switch (who)
    {
    case RUSAGE_SELF:
    {
        GetSysTime(&clock);
        clock.Seconds -= __global_clib2->clock.Seconds;
        if (__global_clib2->clock.Microseconds < clock.Microseconds) {
            clock.Microseconds += 1000000;
            clock.Microseconds -= __global_clib2->clock.Microseconds;
            clock.Seconds--;
        }

        memcpy(rusage, &__global_clib2->ru, sizeof(struct rusage));
        rusage->ru_utime.tv_sec = clock.Seconds;
        rusage->ru_utime.tv_usec = clock.Microseconds;
    }
    break;

    case RUSAGE_CHILDREN:
        memcpy(rusage, &__global_clib2->ru, sizeof(struct rusage));
        break;

    default:
        __set_errno(EINVAL);
        status = -1;
        break;
    }

    return status;
}