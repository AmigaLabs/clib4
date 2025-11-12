/*
 * $Id: stdlib_putenv.c,v 1.8 2025-11-02 12:04:26 clib4devs Exp $
*/
#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

int
putenv(const char *string) {
    int result = ERROR;
    register char *p, *equal;
    struct _clib4 *__clib4 = __CLIB4;

    ENTER();

    SHOWSTRING(string);

    assert(string != NULL);

    __check_abort();

    if (string == NULL) {
        SHOWMSG("invalid string");

        __set_errno_r(__clib4, EFAULT);
        goto out;
    }

    p = strdup(string);
    if (!p) {
        __set_errno_r(__clib4, ENOMEM);
        goto out;
    }

    if (!(equal = index(p, '='))) {
        __set_errno_r(__clib4, EINVAL);
        free(p);
        goto out;
    }

    *equal = '\0';
    result = setenv(p, equal + 1, 1);
    __free_r(__clib4, p);

out:

    RETURN(result);
    return (result);
}
