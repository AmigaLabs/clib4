/*
 * $Id: time_gmtime_r.c,v 1.4 2006-01-08 12:04:27 clib2devs Exp $
*/

#ifndef _TIME_HEADERS_H
#include "time_headers.h"
#endif /* _TIME_HEADERS_H */

#ifndef _STDLIB_PROTOS_H
#include "stdlib_protos.h"
#endif /* _STDLIB_PROTOS_H */

struct tm *
gmtime_r(const time_t *t, struct tm *tm_ptr) {
    struct tm *result = NULL;

    ENTER();

    assert(t != NULL && tm_ptr != NULL);

    if (t == NULL || tm_ptr == NULL) {
        __set_errno(EFAULT);
        goto out;
    }

    result = __convert_time((*t), 0, tm_ptr);

out:

    RETURN(result);
    return (result);
}
