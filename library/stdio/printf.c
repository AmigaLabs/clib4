/*
 * $Id: stdio_printf.c,v 1.6 2006-01-08 12:04:24 clib2devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

int
printf(const char *format, ...) {
    int result = EOF;
    va_list arg;
    ENTER();
    SHOWSTRING(format);

    assert(format != NULL);

    if (format == NULL) {
        __set_errno(EFAULT);
        goto out;
    }

    __stdio_lock();
    va_start(arg, format);
    result = vfprintf(stdout, format, arg);
    va_end(arg);

    __stdio_unlock();

out:

    RETURN(result);
    return (result);
}
