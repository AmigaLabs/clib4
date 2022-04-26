/*
 * $Id: time_gmtime.c,v 1.5 2006-01-08 12:04:27 clib2devs Exp $
*/

#ifndef _TIME_HEADERS_H
#include "time_headers.h"
#endif /* _TIME_HEADERS_H */

struct tm *
gmtime(const time_t *t) {
    static struct tm tm;
    struct tm *result;

    ENTER();

    result = gmtime_r(t, &tm);

    RETURN(result);
    return (result);
}
