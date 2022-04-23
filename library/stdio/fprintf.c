/*
 * $Id: stdio_fprintf.c,v 1.7 2006-01-08 12:04:24 clib2devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

int
fprintf(FILE *stream, const char *format, ...) {
    int result = EOF;
    va_list arg;

    ENTER();

    SHOWPOINTER(stream);
    SHOWSTRING(format);

    assert(stream != NULL && format != NULL);

    if (__check_abort_enabled)
        __check_abort();

    if (stream == NULL || format == NULL) {
        __set_errno(EFAULT);
        goto out;
    }

    va_start(arg, format);
    result = vfprintf(stream, format, arg);
    va_end(arg);

out:

    RETURN(result);
    return (result);
}
