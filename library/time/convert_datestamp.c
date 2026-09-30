/*
 * $Id: time_convert_datestamp.c,v 1.3 2006-01-08 12:04:27 clib4devs Exp $
*/

#ifndef _TIME_HEADERS_H
#include "time_headers.h"
#endif /* _TIME_HEADERS_H */

#ifndef _LOCALE_HEADERS_H
#include "locale_headers.h"
#endif /* _LOCALE_HEADERS_H */

/* The UTC offset, in minutes west of Greenwich, used to convert between the
   local-time DateStamps AmigaOS stores on files and Unix UTC time_t.

   This MUST come from the same place gettimeofday() takes it from, otherwise
   every file timestamp is skewed against the clock: a file written moments ago
   reads back minutes -- or hours -- in the future, and anything that compares a
   file's mtime with the current time draws the wrong conclusion (lock files,
   java.util.prefs, make-style freshness checks).

   gettimeofday() reads timezone.library (GetTimezoneAttrs/TZA_UTCOffset), which
   is the authority on AmigaOS 4 and is what Prefs/Time sets.  locale.library's
   loc_GMTOffset is the legacy setting and is routinely left at 0 on systems
   whose Timezone prefs are perfectly correct -- which is how the two came to
   disagree here.  Fall back to it only when timezone.library is unavailable.

   TZA_UTCOffset alone is deliberate: it is the *current* effective offset, and
   using exactly what gettimeofday() uses is the property that matters. */
int32
__get_gmt_offset(void) {
    struct _clib4 *__clib4 = __CLIB4;
    int32 gmtoffset = 0;

    DECLARE_TIMEZONEBASE_R(__clib4);

    if (ITimezone) {
        GetTimezoneAttrs(NULL, TZA_UTCOffset, &gmtoffset, TAG_DONE);
        return gmtoffset;
    }

    __locale_lock(__clib4);

    if (__clib4->__default_locale != NULL)
        gmtoffset = __clib4->__default_locale->loc_GMTOffset;

    __locale_unlock(__clib4);

    return gmtoffset;
}

time_t
__convert_datestamp_to_time(const struct DateStamp *ds) {
    time_t result;

    ENTER();

    /* Adjust for the local time zone: AmigaOS keeps file dates in local time
       and we return UTC. */
    result = UNIX_TIME_OFFSET + ds->ds_Days * (24 * 60 * 60) + ds->ds_Minute * 60 + (ds->ds_Tick / TICKS_PER_SECOND);
    result += 60 * __get_gmt_offset();

    RETURN(result);
    return (result);
}
