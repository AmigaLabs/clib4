/*
 * $Id: stdlib_putenv.c,v 1.7 2006-01-08 12:04:26 clib2devs Exp $
*/
#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

int
putenv(const char *string) {
    int result = ERROR;

    ENTER();

    SHOWSTRING(string);

    assert(string != NULL);

    __check_abort();

    if (string == NULL) {
        SHOWMSG("invalid string");

        __set_errno(EFAULT);
        goto out;
    }

    result = setenv(string, "", 1);

out:

    RETURN(result);
    return (result);
}
