/*
 * $Id: stdio_getchar_unlocked.c,v 1.3 2006-01-08 12:04:24 clib2devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#undef getchar_unlocked

int
getchar_unlocked(void) {
    int result = EOF;

    ENTER();

    assert(stdin != NULL);

    if (stdin == NULL) {
        __set_errno(EFAULT);
        goto out;
    }

    result = __getc_unlocked(stdin);

out:

    RETURN(result);
    return (result);
}
