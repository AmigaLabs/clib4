/*
 * $Id: stdlib_atoi.c,v 1.4 2006-01-08 12:04:25 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

int
atoi(const char *str) {
    int result = 0;

    ENTER();
    SHOWSTRING(str);

    assert(str != NULL);

    if (str == NULL) {
        __set_errno(EFAULT);
        goto out;
    }

    result = (int) strtol(str, (char **) NULL, 10);

out:
    RETURN(result);
    return (result);
}
