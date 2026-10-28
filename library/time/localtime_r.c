/*
 * $Id: time_localtime_r.c,v 1.6 2006-01-08 12:04:27 clib4devs Exp $
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
    struct _clib4 *__clib4 = __CLIB4;

    ENTER();

    assert(t != NULL && tm_ptr != NULL);

    if (t == NULL || tm_ptr == NULL) {
        __set_errno_r(__clib4, EFAULT);
        goto out;
    }

    /* The time parameter given represents UTC and
     * must be converted to local time before we proceed:
     * the offset at that time, summer time included.
     */
    {
        ULONG amiga_seconds = (ULONG)(*t) > UNIX_TIME_OFFSET ? (ULONG)(*t) - UNIX_TIME_OFFSET : 0;
        int isdst = -1;
        const char *zone = NULL;

        gmt_offset = __time_offset(__clib4, amiga_seconds, TRUE, &isdst, &zone);
        SHOWVALUE(gmt_offset);
        result = __convert_time(__clib4, (*t), gmt_offset, tm_ptr);
        if (result != NULL) {
            tm_ptr->tm_isdst = isdst;
            tm_ptr->tm_zone = zone != NULL ? zone : "GMT";
            tm_ptr->tm_gmtoff = -gmt_offset;
        }
    }

out:

    RETURN(result);
    return (result);
}
