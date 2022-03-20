/*
 * $Id: stdio_fscanf.c,v 1.7 2006-01-08 12:04:24 clib2devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */


int
fscanf(FILE *stream, const char *format, ...) {
    int result = EOF;
    va_list arg;

    ENTER();

    SHOWPOINTER(stream);
    SHOWSTRING(format);

    assert(stream != NULL && format != NULL);

    if (__check_abort_enabled)
        __check_abort();

    if (stream == NULL || format == NULL) {
        SHOWMSG("invalid parameters");

        __set_errno(EFAULT);
        goto out;
    }

    va_start(arg, format);
    result = vfscanf(stream, format, arg);
    va_end(arg);

out:

    RETURN(result);
    return (result);
}
