/*
 * $Id: time_gettimeofday.c,v 1.12 2006-12-28 14:30:57 clib2devs Exp $
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
gettimeofday(struct timeval *tp, struct timezone *tzp) {
    struct _clib2 *__clib2 = __CLIB2;
    int32 gmtoffset = 0;
    int8 dstime = -1;
    ULONG seconds, microseconds;

    DECLARE_TIMERBASE();
    DECLARE_TIMEZONEBASE();

    ENTER();

    if (!__clib2->__ITimer) {
        tp->tv_sec = tp->tv_usec = tzp->tz_minuteswest = tzp->tz_dsttime = 0;
        RETURN(0);
    }

    ObtainSemaphore(__clib2->__timer_semaphore);

    GetTimezoneAttrs(NULL, TZA_UTCOffset, &gmtoffset, TZA_TimeFlag, &dstime, TAG_DONE);

    /* Obtain the current system time. */
#if defined(__NEW_TIMEVAL_DEFINITION_USED__)
    {
        struct TimeVal tv;

        GetSysTime(&tv);

        seconds = tv.Seconds;
        microseconds = tv.Microseconds;
    }
#else
    {
        struct timeval tv;

        GetSysTime(&tv);

        seconds = tv.tv_sec;
        microseconds = tv.tv_usec;
    }
#endif /* __NEW_TIMEVAL_DEFINITION_USED__ */

    ReleaseSemaphore(__clib2->__timer_semaphore);

    /* Convert the number of seconds so that they match the Unix epoch, which
       starts (January 1st, 1970) eight years before the AmigaOS epoch. */
    seconds += UNIX_TIME_OFFSET;

    /* If possible, adjust for the local time zone. We do this because the
       AmigaOS system time is returned in local time and we want to return
       it in UTC. */
    seconds += 60 * gmtoffset;

    if (tp != NULL) {
        tp->tv_sec = (long) seconds;
        tp->tv_usec = (long) microseconds;

        SHOWVALUE(tp->tv_sec);
        SHOWVALUE(tp->tv_usec);
    }

    if (tzp != NULL) {
        tzp->tz_minuteswest = gmtoffset;
        tzp->tz_dsttime = dstime;

        SHOWVALUE(tzp->tz_minuteswest);
        SHOWVALUE(tzp->tz_dsttime);
    }

    RETURN(0);
    return (0);
}
