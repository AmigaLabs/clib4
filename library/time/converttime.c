/*
 * $Id: time_converttime.c,v 2.1 2026-10-28 12:04:27 clib4devs Exp $
*/

#ifndef _TIME_HEADERS_H
#include "time_headers.h"
#endif /* _TIME_HEADERS_H */

#ifndef _LOCALE_HEADERS_H
#include "locale_headers.h"
#endif /* _LOCALE_HEADERS_H */

/* Seconds from the start of the year SECONDS (AmigaOS seconds) falls in. */
static ULONG
__year_position(ULONG seconds) {
    struct ClockData cd;
    DECLARE_UTILITYBASE();

    Amiga2Date(seconds, &cd);
    cd.month = 1;
    cd.mday = 1;
    cd.hour = cd.min = cd.sec = 0;
    return seconds - Date2Amiga(&cd);
}

/*
 * The offset of local time from UTC, in seconds west, at the time SECONDS
 * (AmigaOS seconds): UTC time when IS_UTC, local standard time otherwise.
 * ISDST, when not NULL and not negative, says whether the local time is in
 * summer time (mktime's tm_isdst); the answer goes back through it and the
 * zone name through ZONE.
 *
 * The offsets and the summer time rule come from timezone.library: the
 * standard and summer offsets and this year's DST start and end. A time of
 * another year is placed by its position in its year, as the rule moves only a
 * few days from one year to the next. Without timezone.library the TZ variable
 * (tzset) and then the locale's GMT offset give a fixed offset.
 */
LONG
__time_offset(struct _clib4 *__clib4, ULONG seconds, BOOL is_utc, int *isdst, const char **zone) {
    DECLARE_TIMEZONEBASE_R(__clib4);
    int32 std_ofs = 0, dst_ofs = 0;
    uint32 start = 0, end = 0;
    LONG offset;
    int in_dst = 0;

    if (ITimezone != NULL) {
        GetTimezoneAttrs(NULL,
                         TZA_UTCOffsetSTD, &std_ofs,
                         TZA_UTCOffsetDST, &dst_ofs,
                         TZA_DSTStart, &start,
                         TZA_DSTEnd, &end,
                         TAG_DONE);

        if (std_ofs != dst_ofs && start != end) {
            /* The rule is in local standard time (start) and summer time (end):
               compare in standard time, with the end moved back by the shift. */
            ULONG local = is_utc ? seconds - 60 * std_ofs : seconds;
            ULONG pos = __year_position(local);
            ULONG spos = __year_position(start);
            ULONG epos = __year_position(end) - 60 * (std_ofs - dst_ofs);

            if (isdst != NULL && *isdst >= 0 && !is_utc)
                in_dst = *isdst > 0;
            else if (spos < epos)
                in_dst = pos >= spos && pos < epos;
            else
                in_dst = pos >= spos || pos < epos;
        }

        offset = 60 * (in_dst ? dst_ofs : std_ofs);
        if (zone != NULL)
            *zone = __clib4->__tzname[in_dst ? 1 : 0];
    }
    else if (__clib4->isTZSet) {
        offset = __clib4->__timezone;
        if (zone != NULL)
            *zone = __clib4->__tzname[0];
    }
    else {
        __locale_lock(__clib4);
        offset = __clib4->__default_locale != NULL ? 60 * __clib4->__default_locale->loc_GMTOffset : 0;
        __locale_unlock(__clib4);
        if (zone != NULL)
            *zone = __clib4->__tzname[0];
    }

    if (isdst != NULL)
        *isdst = in_dst;
    return offset;
}

/*
 * The broken-down time of SECONDS (UTC, Unix seconds) at GMT_OFFSET seconds
 * west of UTC. The caller sets tm_isdst, tm_zone and tm_gmtoff; here they are
 * left as for UTC.
 */
struct tm *
__convert_time(struct _clib4 *__clib4, ULONG seconds, LONG gmt_offset, struct tm *tm) {
    struct ClockData clock_data;
    struct tm *result;
    DECLARE_UTILITYBASE();

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
    seconds -= gmt_offset;

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
    tm->tm_isdst = 0;
    tm->tm_zone = "GMT";
    tm->tm_gmtoff = 0;

    /* Now figure out how many days have passed since January 1st. */
    tm->tm_yday = __calculate_days_per_date(clock_data.year, clock_data.month, clock_data.mday) -
                  __calculate_days_per_date(clock_data.year, 1, 1);

    result = tm;

    RETURN(result);
    return (result);
}
