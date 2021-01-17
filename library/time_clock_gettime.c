/*
 * $Id: time_gettime.c,v 1.0 2020-01-13 16:55:42 apalmate Exp $
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

int clock_gettime(clockid_t clk_id, struct timespec *t)
{
    /* Check the supported flags.  */
    if ((clk_id & ~(CLOCK_MONOTONIC | CLOCK_REALTIME)) != 0)
    {
        __set_errno(EINVAL);
        RETURN(-1);
        return -1;
    }

    DECLARE_TIMEZONEBASE();

    struct timeval tv;
    int result = 0;
    struct timerequest *tr = NULL;
    uint32 gmtoffset = 0;
    int8 dstime = -1;

    //Set default value for tv
    tv.tv_secs = tv.tv_micro = 0;

#ifdef __amigaos4__
    GetTimezoneAttrs(NULL, TZA_UTCOffset, &gmtoffset, TZA_TimeFlag, &dstime, TAG_DONE);
#endif

    if (result == 0)
    {
        struct MsgPort *Timer_Port;
#ifdef __amigaos4__
        Timer_Port = AllocSysObjectTags(ASOT_PORT, ASOPORT_AllocSig, FALSE, ASOPORT_Signal, SIGB_SINGLE, TAG_DONE);
#else
        Timer_Port = AllocVec(sizeof(*Timer_Port), MEMF_ANY | MEMF_PUBLIC | MEMF_CLEAR);
#endif
        if (Timer_Port == NULL)
        {
            SHOWMSG("Cannot create Timer port\n");
            result = -1;
        }
        else
        {
#ifdef __amigaos4__
            tr = AllocSysObjectTags(ASOT_IOREQUEST, ASOIOR_Size, sizeof(struct TimeRequest), ASOIOR_ReplyPort, Timer_Port, TAG_END);
#else
            tr = (struct timerequest *)CreateIORequest(Timer_Port, sizeof(*tr));
#endif
            if (tr != NULL)
            {
                if (!(OpenDevice(TIMERNAME, UNIT_MICROHZ, (struct IORequest *)tr, 0)))
                {
                    struct Library *TimerBase = (struct Library *)tr->tr_node.io_Device;
#if defined(__amigaos4__)
                    struct TimerIFace *ITimer = (struct TimerIFace *)GetInterface(TimerBase, "main", 1, NULL);
#endif
                    if (ITimer != NULL)
                    {
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

#if defined(__amigaos4__)
                        DropInterface((struct Interface *)ITimer);
#endif
                    }
                    else
                    {
                        SHOWMSG("Cannot get Timer interface\n");
                        result = -1;
                    }
                    CloseDevice((struct IORequest *)tr);
                }
                else
                    result = -1;
#ifdef __amigaos4__
                FreeSysObject(ASOT_IOREQUEST, tr);
#else
                DeleteIORequest(tr);
#endif
            }
            else
            {
                SHOWMSG("Cannot allocate IORequest\n");
                result = -1;
            }
#ifdef __amigaos4__
            FreeSysObject(ASOT_PORT, Timer_Port);
#else
            FreeVec(Timer_Port);
#endif
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
    return result;
}
