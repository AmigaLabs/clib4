/*
 * $Id: time_clock_gettime64.c,v 1.0 2023-03-03 16:55:42 clib4devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#ifndef _TIMEZONE_HEADERS_H
#include "timezone_headers.h"
#endif /* _TIMEZONE_HEADERS_H */

int
clock_gettime64(clockid_t clk_id, struct timespec64 *t) {
    ENTER();
    struct _clib4 *__clib4 = __CLIB4;

    /* Check the supported flags.  */
    if ((clk_id & ~(CLOCK_MONOTONIC | CLOCK_REALTIME)) != 0) {
        __set_errno(EINVAL);
        RETURN(-1);
        return -1;
    }

	/* Our clock is alwayy present for reading */

    DECLARE_TIMERBASE_R(__clib4);
    DECLARE_TIMEZONEBASE_R(__clib4);

    struct timeval tv;
    uint32 gmtoffset = 0;
    int8 dstime = -1;

    //Set default value for tv
    tv.tv_sec = tv.tv_usec = 0;

    GetTimezoneAttrs(NULL, TZA_UTCOffset, &gmtoffset, TZA_TimeFlag, &dstime, TAG_DONE);
    
	if (clk_id == CLOCK_MONOTONIC) {
        /*
        CLOCK_MONOTONIC
            A nonsettable system-wide clock that represents monotonic
            time since—as described by POSIX—"some unspecified point
            in the past". On clib4, that point corresponds to the
            number of seconds that the system has been running since
            it was booted.
        */
        GetUpTime((struct TimeVal *) &tv);
    } else {
        /*
        A settable system-wide clock that measures real (i.e., wall-clock) time.
        */
        GetSysTime((struct TimeVal *) &tv);
    }

    /* Use a 32 bit timespec to calculate the result */
    struct timespec tr;
    if (clk_id == CLOCK_MONOTONIC) {
        tr.tv_sec = tv.tv_sec;
        tr.tv_nsec = tv.tv_usec * 1000;
    } else {
        /* 2922 is the number of days between 1.1.1970 and 1.1.1978 */
        tv.tv_sec += (2922 * 24 * 60 + gmtoffset) * 60;
        tr.tv_sec = tv.tv_sec;
        tr.tv_nsec = tv.tv_usec * 1000;
    }

    /* And then convert it to a 64bit timespec */
    *t = valid_timespec_to_timespec64(tr);

    RETURN(0);
    return 0;
}
