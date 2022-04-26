/*
 * $Id: stdio_putc_unlocked.c,v 1.3 2006-01-08 12:04:25 clib2devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#undef putc_unlocked

int
putc_unlocked(int c, FILE *stream) {
    int result = EOF;

    ENTER();
    SHOWVALUE(c);
    SHOWPOINTER(stream);

    assert(stream != NULL);

    if (stream == NULL) {
        __set_errno(EFAULT);
        goto out;
    }

    result = __putc_unlocked(c, stream);

out:

    RETURN(result);
    return (result);
}
