/*
 * $Id: time_convert_time.c,v 1.3 2006-01-08 12:04:27 clib2devs Exp $
*/

#ifndef _TIME_HEADERS_H
#include "time_headers.h"
#endif /* _TIME_HEADERS_H */

#ifndef _LOCALE_HEADERS_H
#include "locale_headers.h"
#endif /* _LOCALE_HEADERS_H */

BOOL
__convert_time_to_datestamp(time_t time_value, struct DateStamp *ds) {
    BOOL success;

    /* Adjust the time to the AmigaOS epoch. */
    time_value -= UNIX_TIME_OFFSET;

    __locale_lock();

    /* If possible, adjust the time to match the local time zone settings. */
    if (__default_locale != NULL)
        time_value -= 60 * __default_locale->loc_GMTOffset;

    __locale_unlock();

    ds->ds_Days = (time_value / (24 * 60 * 60));
    ds->ds_Minute = (time_value % (24 * 60 * 60)) / 60;
    ds->ds_Tick = (time_value % 60) * TICKS_PER_SECOND;

    success = TRUE;

    return (success);
}
