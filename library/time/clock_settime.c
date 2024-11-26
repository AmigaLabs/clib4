/*
 * $Id: time_settime.c,v 1.0 2020-01-13 17:20:12 clib4devs Exp $
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

int
clock_settime(clockid_t clk_id, const struct timespec *t) {
    ENTER();
    struct _clib4 *__clib4 = __CLIB4;

    int result = -1;

    if ((clk_id & ~(CLOCK_MONOTONIC | CLOCK_REALTIME)) != 0) {
        __set_errno_r(__clib4, EINVAL);
        RETURN(-1);
        return -1;
    }

    if (__clib4->__timer_busy) {
        __set_errno_r(__clib4, EAGAIN);
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
            __clib4->__timer_busy = TRUE;
            __clib4->__timer_request->Request.io_Message.mn_ReplyPort = __clib4->__timer_port;
            __clib4->__timer_request->Request.io_Command = TR_SETSYSTIME;
            /* 2922 is the number of days between 1.1.1970 and 1.1.1978 */
            __clib4->__timer_request->Time.Seconds = t->tv_sec - ((2922 * 24 * 60 + __gmtoffset) * 60);
            __clib4->__timer_request->Time.Microseconds = t->tv_nsec / 1000;

            DoIO((struct IORequest *) __clib4->__timer_request);
            GetMsg(__clib4->__timer_port);

            result = 0;
            __set_errno_r(__clib4, 0);
        }
            break;

        case CLOCK_MONOTONIC: // TODO - Are CLOCK_MONOTONIC and CLOCK_MONOTONIC_RAW settable?
        case CLOCK_MONOTONIC_RAW: {
            struct timeval tv;
            TIMESPEC_TO_TIMEVAL(&tv, t);
            __clib4->clock.tv_sec = tv.tv_sec;
            __clib4->clock.tv_usec = tv.tv_usec;
        }
            break;

        default:
            __set_errno_r(__clib4, EINVAL);
            break;
    }

    __clib4->__timer_busy = FALSE;

    RETURN(result);
    return result;
}
