/*
 * $Id: time_mktime.c,v 1.11 2015-06-26 11:22:00 clib4devs Exp $
*/

#ifndef _TIME_HEADERS_H
#include "time_headers.h"
#endif /* _TIME_HEADERS_H */

#ifndef _LOCALE_HEADERS_H
#include "locale_headers.h"
#endif /* _LOCALE_HEADERS_H */

time_t
mktime(struct tm *tm) {
    DECLARE_UTILITYBASE();
    struct ClockData clock_data;
    ULONG seconds;
    time_t result = (time_t) - 1;
    LONG combined_seconds;
    int month, year;
    struct _clib4 *__clib4 = __CLIB4;

    ENTER();

    assert(tm != NULL);
    assert(UtilityBase != NULL);

    if (tm == NULL) {
        SHOWMSG("invalid tm parameter");

        __set_errno_r(__clib4, EFAULT);
        goto out;
    }

    /* Normalize the year and month. */
    year = tm->tm_year + 1900;
    month = tm->tm_mon + 1;

    if (month < 0 || month > 12) {
        int y;

        y = month / 12;

        month -= y * 12;
        year += y;
    }

    if (month < 1) {
        month += 12;
        year -= 1;
    }

    /* The year must be valid. Amiga time begins with January 1st, 1978. */
    if (year < 1978) {
        SHOWVALUE(year);
        SHOWMSG("invalid year");
        goto out;
    }

    /* Convert the first day of the month in the given year
       into the corresponding number of seconds. */
    memset(&clock_data, 0, sizeof(clock_data));

    clock_data.mday = 1;
    clock_data.month = month;
    clock_data.year = year;

    seconds = Date2Amiga(&clock_data);

    /* Put the combined number of seconds involved together,
       covering the seconds/minutes/hours of the day as well
       as the number of days of the month. This will be added
       to the number of seconds for the date. */
    combined_seconds = tm->tm_sec + 60 * (tm->tm_min + 60 * (tm->tm_hour + 24 * (tm->tm_mday - 1)));

    /* If the combined number of seconds is negative, adding it
     * to the number of seconds for the date should not produce
     * a negative value.
     */
    if (combined_seconds < 0 && seconds < (ULONG)(-combined_seconds)) {
        SHOWVALUE(seconds);
        SHOWVALUE(combined_seconds);
        SHOWMSG("invalid combined number of seconds");
        goto out;
    }

    seconds += combined_seconds;

    __locale_lock(__clib4);

    /* The data in 'struct tm *tm' was given in local time. We need
       to convert the result into UTC. */
    if (__clib4->__default_locale != NULL)
        seconds += 60 * __clib4->__default_locale->loc_GMTOffset;

    __locale_unlock(__clib4);

    /* Adjust for the difference between the Unix and the
       AmigaOS epochs, which differ by 8 years. */
    result = seconds + UNIX_TIME_OFFSET;

    /* Finally, normalize the provided time and date information. */
    localtime_r(&result, tm);

out:

    RETURN(result);
    return (result);
}