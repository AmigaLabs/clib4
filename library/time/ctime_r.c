/*
 * $Id: time_ctime_r.c,v 1.4 2006-01-08 12:04:27 clib2devs Exp $
*/

#ifndef _TIME_HEADERS_H
#include "time_headers.h"
#endif /* _TIME_HEADERS_H */

#ifndef _STDLIB_PROTOS_H
#include "stdlib_protos.h"
#endif /* _STDLIB_PROTOS_H */

char *
ctime_r(const time_t *tptr, char *buffer) {
    char *result = NULL;
    struct tm tm;

    ENTER();

    assert(tptr != NULL && buffer != NULL);

    if (tptr == NULL || buffer == NULL) {
        __set_errno(EFAULT);
        goto out;
    }

    result = asctime_r(localtime_r(tptr, &tm), buffer);

out:

    RETURN(result);
    return (result);
}
