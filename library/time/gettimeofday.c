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
    struct TimerIFace *ITimer = __clib2->__ITimer;

    ULONG seconds, microseconds;

    ENTER();

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

    /* Convert the number of seconds so that they match the Unix epoch, which
       starts (January 1st, 1970) eight years before the AmigaOS epoch. */
    seconds += UNIX_TIME_OFFSET;

    __locale_lock();

    /* If possible, adjust for the local time zone. We do this because the
       AmigaOS system time is returned in local time and we want to return
       it in UTC. */
    if (__clib2->__default_locale != NULL)
        seconds += 60 * __clib2->__default_locale->loc_GMTOffset;

    if (tp != NULL) {
        tp->tv_sec = (long) seconds;
        tp->tv_usec = (long) microseconds;

        SHOWVALUE(tp->tv_sec);
        SHOWVALUE(tp->tv_usec);
    }

    if (tzp != NULL) {
        if (__clib2->__default_locale != NULL)
            tzp->tz_minuteswest = __clib2->__default_locale->loc_GMTOffset;
        else
            tzp->tz_minuteswest = 0;

        /* The -1 means "we do not know if the time given is in
           daylight savings time". */
        tzp->tz_dsttime = -1;

        SHOWVALUE(tzp->tz_minuteswest);
        SHOWVALUE(tzp->tz_dsttime);
    }

    __locale_unlock();

    RETURN(0);
    return (0);
}
