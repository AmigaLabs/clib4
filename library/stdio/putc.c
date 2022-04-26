/*
 * $Id: stdio_putc.c,v 1.5 2006-01-08 12:04:24 clib2devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#undef putc

int
putc(int c, FILE *stream) {
    int result = EOF;

    ENTER();

    assert(stream != NULL);

    if (stream == NULL) {
        __set_errno(EFAULT);
        goto out;
    }

    result = fputc(c, stream);

out:

    LEAVE();
    return (result);
}
