/*
 * $Id: time_converttime.c,v 2.0 2023-05-26 12:04:27 clib4devs Exp $
*/

#ifndef _TIME_HEADERS_H
#include "time_headers.h"
#endif /* _TIME_HEADERS_H */

#ifndef _LOCALE_HEADERS_H
#include "locale_headers.h"
#endif /* _LOCALE_HEADERS_H */

struct tm *
__convert_time(ULONG seconds, LONG gmt_offset, struct tm *tm) {
    struct ClockData clock_data;
    struct tm *result;
    struct _clib4 *__clib4 = __CLIB4;
    DECLARE_UTILITYBASE();
    DECLARE_TIMEZONEBASE_R(__clib4);
    int8 dstime = -1;

    ENTER();

    assert(UtilityBase != NULL);

    /* We need to convert the time from Unix-style UTC
     * back into Amiga style local time.
     *
     * First, the Unix time offset will have to go.
     */
    if (seconds < UNIX_TIME_OFFSET)
        seconds = 0;
    else
        seconds -= UNIX_TIME_OFFSET;

    /* Now the local time offset will have to go. */
    if (gmt_offset > 0)
        seconds -= gmt_offset;
    else
        seconds += gmt_offset;

    /* Check if we are in DST */
    GetTimezoneAttrs(NULL, TZA_TimeFlag, &dstime, TAG_DONE);
    if (dstime == TFLG_ISDST)
        seconds += (60 * 60);

    /* Convert the number of seconds into a more useful format. */
    Amiga2Date(seconds, &clock_data);

    /* The 'struct clockdata' layout and contents are very similar
     * to the 'struct tm' contents. We don't have to convert much,
     * except for the 'tm.tm_yday' field below.
     */
    tm->tm_sec = clock_data.sec;
    tm->tm_min = clock_data.min;
    tm->tm_hour = clock_data.hour;
    tm->tm_mday = clock_data.mday;
    tm->tm_mon = clock_data.month - 1;
    tm->tm_year = clock_data.year - 1900;
    tm->tm_wday = clock_data.wday;
    tm->tm_isdst = __clib4->__daylight;
    tm->tm_zone = __clib4->__tzname[0];
    tm->tm_gmtoff = __clib4->__timezone / 60;

    /* Now figure out how many days have passed since January 1st. */
    tm->tm_yday = __calculate_days_per_date(clock_data.year, clock_data.month, clock_data.mday) -
                  __calculate_days_per_date(clock_data.year, 1, 1);

    result = tm;

    RETURN(result);
    return (result);
}
