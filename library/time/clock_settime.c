/*
 * $Id: time_settime.c,v 1.0 2020-01-13 17:20:12 clib2devs Exp $
*/

#define _GNU_SOURCE

#ifndef _TIME_HEADERS_H

#include "time_headers.h"

#endif /* _TIME_HEADERS_H */

#ifndef _STDIO_HEADERS_H

#include "stdio_headers.h"

#endif /* _STDIO_HEADERS_H */

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

extern struct MsgPort NOCOMMON *__timer_port;
extern struct TimeRequest NOCOMMON *__timer_request;
extern BOOL NOCOMMON __timer_busy;

int
clock_settime(clockid_t clk_id, const struct timespec *t) {
    ENTER();

    int result = -1;

    if ((clk_id & ~(CLOCK_MONOTONIC | CLOCK_REALTIME)) != 0) {
        __set_errno(EINVAL);
        RETURN(-1);
        return -1;
    }

    if (__timer_busy) {
        __set_errno(EAGAIN);
        RETURN(result);
        return result;
    }


    DECLARE_TIMEZONEBASE();

    switch (clk_id) {
        case CLOCK_REALTIME: {
            int32 __gmtoffset = 0;
            int8 __dstime = -1;

            if (ITimezone) {
                GetTimezoneAttrs(NULL, TZA_UTCOffset, &__gmtoffset, TZA_TimeFlag, &__dstime, TAG_DONE);
            }
            __timer_busy = TRUE;
            __timer_request->Request.io_Message.mn_ReplyPort = __timer_port;
            __timer_request->Request.io_Command = TR_SETSYSTIME;
            /* 2922 is the number of days between 1.1.1970 and 1.1.1978 */
            __timer_request->Time.Seconds = t->tv_sec - ((2922 * 24 * 60 + __gmtoffset) * 60);
            __timer_request->Time.Microseconds = t->tv_nsec / 1000;

            DoIO((struct IORequest *) __timer_request);
            GetMsg(__timer_port);

            result = 0;
            __set_errno(0);
        }
            break;

        case CLOCK_MONOTONIC: {
            struct timeval tv;
            TIMESPEC_TO_TIMEVAL(&tv, t);
            __global_clib2->clock.Seconds = tv.tv_sec;
            __global_clib2->clock.Microseconds = tv.tv_usec;
        }
            break;

        default:
            __set_errno(EINVAL);
            break;
    }

    __timer_busy = FALSE;

    RETURN(result);
    return result;
}
