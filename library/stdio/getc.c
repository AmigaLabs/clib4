/*
 * $Id: stdio_getc.c,v 1.5 2006-01-08 12:04:24 clib2devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#undef getc

int
getc(FILE *stream) {
    int result = EOF;

    ENTER();

    assert(stream != NULL);

    if (stream == NULL) {
        __set_errno(EFAULT);
        goto out;
    }

    result = fgetc(stream);

out:

    LEAVE();

    return (result);
}
