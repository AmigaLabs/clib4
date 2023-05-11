/*
 * $Id: time_localtime_r.c,v 1.6 2006-01-08 12:04:27 clib2devs Exp $
*/

#ifndef _TIME_HEADERS_H
#include "time_headers.h"
#endif /* _TIME_HEADERS_H */

#ifndef _LOCALE_HEADERS_H
#include "locale_headers.h"
#endif /* _LOCALE_HEADERS_H */

struct tm *
localtime_r(const time_t *t, struct tm *tm_ptr) {
    struct tm *result = NULL;
    LONG gmt_offset;
    struct _clib2 *__clib2 = __CLIB2;

    ENTER();

    assert(t != NULL && tm_ptr != NULL);

    if (t == NULL || tm_ptr == NULL) {
        __set_errno(EFAULT);
        goto out;
    }

    __locale_lock();

    /* The time parameter given represents UTC and
     * must be converted to local time before we proceed.
     */
    if (__clib2->__default_locale != NULL)
        gmt_offset = 60 * __clib2->__default_locale->loc_GMTOffset;
    else
        gmt_offset = 0;

    __locale_unlock();

    SHOWVALUE(gmt_offset);

    result = __convert_time((*t), gmt_offset, tm_ptr);

out:

    RETURN(result);
    return (result);
}
