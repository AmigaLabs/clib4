/*
 * $Id: time_clock_gettime.c,v 2.0 2023-05-26 16:55:42 clib4devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#ifndef _TIMEZONE_HEADERS_H
#include "timezone_headers.h"
#endif /* _TIMEZONE_HEADERS_H */

int
clock_gettime(clockid_t clk_id, struct timespec *t) {
    ENTER();
    struct _clib4 *__clib4 = __CLIB4;

    /* Check the supported flags.  */
    if ((clk_id & ~(CLOCK_MONOTONIC | CLOCK_REALTIME | CLOCK_MONOTONIC_RAW)) != 0) {
        __set_errno_r(__clib4, EINVAL);
        RETURN(-1);
        return -1;
    }

    if (__clib4->__timer_busy) {
        __set_errno_r(__clib4, EAGAIN);
        RETURN(-1);
        return -1;
    }

    DECLARE_TIMEZONEBASE();
    struct TimerIFace *ITimer = __clib4->__ITimer;

    struct timeval tv;
    uint32 gmtoffset = 0;
    int8 dstime = -1;

    //Set default value for tv
    tv.tv_sec = tv.tv_usec = 0;

    GetTimezoneAttrs(NULL, TZA_UTCOffset, &gmtoffset, TZA_TimeFlag, &dstime, TAG_DONE);

    __clib4->__timer_busy = TRUE;
    if (clk_id == CLOCK_MONOTONIC | clk_id == CLOCK_MONOTONIC_RAW) {
        /*
        CLOCK_MONOTONIC
            A nonsettable system-wide clock that represents monotonic
            time since—as described by POSIX—"some unspecified point
            in the past". On clib4, that point corresponds to the
            number of seconds that the system has been running since
            it was booted.

        CLOCK_MONOTONIC_RAW
              Similar  to  CLOCK_MONOTONIC, but provides access to a raw hard‐
              ware-based time that is not subject to NTP adjustments.
        */

        GetUpTime((struct TimeVal *) &tv);
    } else {
        /*
        A settable system-wide clock that measures real (i.e., wall-clock) time.
        */
        GetSysTime((struct TimeVal *) &tv);
    }

    if (clk_id == CLOCK_MONOTONIC || clk_id == CLOCK_MONOTONIC_RAW) {
        t->tv_sec = tv.tv_sec;
        t->tv_nsec = tv.tv_usec * 1000;
    } else {
        /* 2922 is the number of days between 1.1.1970 and 1.1.1978 */
        tv.tv_sec += (2922 * 24 * 60 + gmtoffset) * 60;
        t->tv_sec = tv.tv_sec;
        t->tv_nsec = tv.tv_usec * 1000;
    }

    /* Check if we are in DST */
    if (dstime == TFLG_ISDST)
        t->tv_sec += (60 * 60);

    __clib4->__timer_busy = FALSE;

    RETURN(0);
    return 0;
}
