/*
 * $Id: time_settime.c,v 1.0 2020-01-13 17:20:12 apalmate Exp $
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

int clock_settime(clockid_t clk_id, const struct timespec *t)
{
    int result = -1;
    __set_errno(EINVAL);

    ENTER();

    switch (clk_id)
    {
    case CLOCK_REALTIME:
    {
        struct TimerIFace *ITimer = (struct TimerIFace *)GetInterface((struct Library *)TimerBase, "main", 1, NULL);
        if (ITimer)
        {
            struct MsgPort *Timer_Port = AllocSysObjectTags(ASOT_PORT, ASOPORT_AllocSig, FALSE, ASOPORT_Signal, SIGB_SINGLE, TAG_DONE);
            if (Timer_Port)
            {
                struct TimeRequest *Time_Req;
                Time_Req = AllocSysObjectTags(ASOT_MESSAGE, ASOMSG_Size, sizeof(struct TimeRequest), ASOMSG_ReplyPort, Timer_Port, TAG_DONE);
                if (Time_Req)
                {
                    if (!OpenDevice(TIMERNAME, UNIT_VBLANK, (struct IORequest *)Time_Req, 0))
                    {
                        int32 __gmtoffset = 0;
                        int8 __dstime = -1;

                        if (ITimezone)
                        {
                            GetTimezoneAttrs(NULL, TZA_UTCOffset, &__gmtoffset, TZA_TimeFlag, &__dstime, TAG_DONE);
                        }

                        Time_Req->Request.io_Message.mn_ReplyPort = Timer_Port;
                        Time_Req->Request.io_Command = TR_SETSYSTIME;
                        /* 2922 is the number of days between 1.1.1970 and 1.1.1978 */
                        Time_Req->Time.Seconds = t->tv_sec - ((2922 * 24 * 60 + __gmtoffset) * 60);
                        Time_Req->Time.Microseconds = t->tv_nsec / 1000;

                        DoIO((struct IORequest *)Time_Req);
                        GetMsg(Timer_Port);
                        CloseDevice((struct IORequest *)Time_Req);

                        result = 0;
                        __set_errno(0);
                    }

                    FreeSysObject(ASOT_MESSAGE, Time_Req);
                }

                FreeSysObject(ASOT_PORT, Timer_Port);
            }
        }
    }
    break;

    default:
        __set_errno(EINVAL);
        break;
    }

    RETURN(result);
    return result;
}
