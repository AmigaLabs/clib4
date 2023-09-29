/*
 * $Id: time_ctime.c,v 1.6 2006-01-08 12:04:27 clib4devs Exp $
*/

#ifndef _TIME_HEADERS_H
#include "time_headers.h"
#endif /* _TIME_HEADERS_H */

#ifndef _STDLIB_PROTOS_H
#include "stdlib_protos.h"
#endif /* _STDLIB_PROTOS_H */

char *
ctime(const time_t *tptr) {
    char *result = NULL;

    ENTER();

    assert(tptr != NULL);

    if (tptr == NULL) {
        SHOWMSG("invalid tptr parameter");

        __set_errno(EFAULT);
        goto out;
    }

    result = asctime(localtime(tptr));

out:

    RETURN(result);
    return (result);
}
