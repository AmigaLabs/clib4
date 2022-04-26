/*
 * $Id: stdio_snprintf.c,v 1.6 2006-01-08 12:04:25 clib2devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

int
snprintf(char *s, size_t size, const char *format, ...) {
    int result = EOF;
    va_list arg;

    ENTER();

    assert(format != NULL);

    SHOWPOINTER(s);
    SHOWVALUE(size);
    SHOWSTRING(format);

    assert(size == 0 || s != NULL);
    assert((int) size >= 0);

    if ((size > 0 && s == NULL) || format == NULL) {
        __set_errno(EFAULT);
        goto out;
    }

    va_start(arg, format);
    result = vsnprintf(s, size, format, arg);
    va_end(arg);

out:

    RETURN(result);
    return (result);
}
