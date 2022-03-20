/*
 * $Id: time_time.c,v 1.5 2006-01-08 12:04:27 clib2devs Exp $
*/

#ifndef _TIME_HEADERS_H
#include "time_headers.h"
#endif /* _TIME_HEADERS_H */

#ifndef _LOCALE_HEADERS_H
#include "locale_headers.h"
#endif /* _LOCALE_HEADERS_H */

time_t
time(time_t *tptr) {
    struct DateStamp ds;
    time_t result;

    DateStamp(&ds);

    /* This converts the DateStamp contents into the number of
       seconds elapsed since January 1st 1970. The time is
       given as relative to UTC, not local time. */
    result = __convert_datestamp_to_time(&ds);

    if (tptr != NULL)
        (*tptr) = result;

    return (result);
}
