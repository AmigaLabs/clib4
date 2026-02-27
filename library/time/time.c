/*
 * $Id: time_time.c,v 1.5 2006-01-08 12:04:27 clib4devs Exp $
*/

#ifndef _TIME_HEADERS_H
#include "time_headers.h"
#endif /* _TIME_HEADERS_H */

#ifndef _LOCALE_HEADERS_H
#include "locale_headers.h"
#endif /* _LOCALE_HEADERS_H */

time_t
time(time_t *tptr) {

    struct timeval now;

    if (gettimeofday(&now, (struct timezone *) 0) >= 0) {
        if (tptr)
            *tptr = now.tv_sec;
        return now.tv_sec;
    }
    return -1;
}
