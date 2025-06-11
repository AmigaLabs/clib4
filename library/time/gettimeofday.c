/*
 * $Id: time_gettimeofday.c,v 1.12 2006-12-28 14:30:57 clib4devs Exp $
*/

#ifndef PROTO_TIMER_H
#include <proto/timer.h>
#endif /* PROTO_TIMER_H */

#ifndef _TIME_HEADERS_H
#include "time_headers.h"
#endif /* _TIME_HEADERS_H */

#ifndef _LOCALE_HEADERS_H
#include "locale_headers.h"
#endif /* _LOCALE_HEADERS_H */

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

int
gettimeofday(struct timeval *tv, struct timezone *tzp) {
    struct _clib4 *__clib4 = __CLIB4;
    int32 gmtoffset = 0;
    int8 dstime = -1;
    ULONG seconds, microseconds;

    DECLARE_TIMERBASE_R(__clib4);
    DECLARE_TIMEZONEBASE_R(__clib4);

    ENTER();
    if (NULL == __clib4->__ITimer) {
        tv->tv_sec = tv->tv_usec = 0;
        if (tzp != NULL)
            tzp->tz_minuteswest = tzp->tz_dsttime = 0;
        RETURN(0);
        return 0;
    }

    if (ITimezone) {
        GetTimezoneAttrs(NULL, TZA_UTCOffset, &gmtoffset, TZA_TimeFlag, &dstime, TAG_DONE);
    }
    /* Obtain the current system time. */
    GetSysTime((struct TimeVal *) tv);

    /* 2922 is the number of days between 1.1.1970 and 1.1.1978 */
    tv->tv_sec += (2922 * 24 * 60 + gmtoffset) * 60;

    if (tzp != NULL) {
        tzp->tz_minuteswest = gmtoffset;
        tzp->tz_dsttime = dstime;

        SHOWVALUE(tzp->tz_minuteswest);
        SHOWVALUE(tzp->tz_dsttime);
    }

    RETURN(0);
    return (0);
}
