/*
 * $Id: time_timegm.c,v 1.0 2023-07-10 12:04:27 clib2devs Exp $
*/
#define _DEFAULT_SOURCE

#ifndef _TIME_HEADERS_H
#include "time_headers.h"
#endif /* _TIME_HEADERS_H */

#ifndef _LOCALE_HEADERS_H
#include "locale_headers.h"
#endif /* _LOCALE_HEADERS_H */

time_t timegm(struct tm *tm) {
    struct tm new;
    long long t = __tm_to_secs(tm);
    if (__secs_to_tm(t, &new) < 0) {
        __set_errno(EOVERFLOW);
        return -1;
    }
    *tm = new;
    tm->tm_isdst = 0;
    tm->__tm_gmtoff = 0;
    tm->__tm_zone = "UTC";
    return t;
}