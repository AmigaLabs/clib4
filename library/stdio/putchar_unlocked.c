/*
 * $Id: stdio_putchar_unlocked.c,v 1.3 2006-01-08 12:04:25 clib4devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#undef putchar_unlocked

int
putchar_unlocked(int c) {
    int result = EOF;

    ENTER();
    SHOWVALUE(c);

    assert(stdout != NULL);

    if (stdout == NULL) {
        __set_errno(EFAULT);
        goto out;
    }

    result = __putc_unlocked(c, stdout);

out:

    RETURN(result);
    return (result);
}
