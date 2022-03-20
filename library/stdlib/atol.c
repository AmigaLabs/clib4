/*
 * $Id: stdlib_atol.c,v 1.4 2006-01-08 12:04:25 clib2devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

long
atol(const char *str) {
    long result = 0;

    assert(str != NULL);

    if (str == NULL) {
        __set_errno(EFAULT);
        goto out;
    }

    result = strtol(str, (char **) NULL, 10);

out:

    return (result);
}
