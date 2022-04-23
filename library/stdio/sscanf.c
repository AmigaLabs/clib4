/*
 * $Id: stdio_sscanf.c,v 1.9 2006-01-08 12:04:25 clib2devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

int
sscanf(const char *s, const char *format, ...) {
    int result = EOF;
    va_list arg;

    ENTER();
    SHOWSTRING(s);
    SHOWSTRING(format);

    assert(s != NULL && format != NULL);

    if (s == NULL || format == NULL) {
        SHOWMSG("invalid parameters");

        __set_errno(EFAULT);
        goto out;
    }

    va_start(arg, format);
    result = vsscanf(s, format, arg);
    va_end(arg);

out:

    RETURN(result);
    return (result);
}
