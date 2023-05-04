/*
 * $Id: time_convert_datestamp.c,v 1.3 2006-01-08 12:04:27 clib2devs Exp $
*/

#ifndef _TIME_HEADERS_H
#include "time_headers.h"
#endif /* _TIME_HEADERS_H */

#ifndef _LOCALE_HEADERS_H
#include "locale_headers.h"
#endif /* _LOCALE_HEADERS_H */

time_t
__convert_datestamp_to_time(const struct DateStamp *ds) {
    time_t result;

    ENTER();

    __locale_lock();

    /* If possible, adjust for the local time zone. We do this because the
       AmigaOS system time is returned in local time and we want to return
       it in UTC. */
    result = UNIX_TIME_OFFSET + ds->ds_Days * (24 * 60 * 60) + ds->ds_Minute * 60 + (ds->ds_Tick / TICKS_PER_SECOND);
    if (__CLIB2->__default_locale != NULL)
        result += 60 * __CLIB2->__default_locale->loc_GMTOffset;

    __locale_unlock();

    RETURN(result);
    return (result);
}
