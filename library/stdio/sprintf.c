/*
 * $Id: stdio_sprintf.c,v 1.6 2006-01-08 12:04:25 clib2devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

int
sprintf(char *s, const char *format, ...) {
    int result = EOF;
    va_list arg;

    ENTER();

    assert(s != NULL && format != NULL);

    SHOWPOINTER(s);
    SHOWMSG(s);
    SHOWSTRING(format);

    __check_abort();

    if (s == NULL || format == NULL) {
        __set_errno(EFAULT);
        goto out;
    }

    va_start(arg, format);
    result = vsprintf(s, format, arg);
    va_end(arg);

out:

    RETURN(result);
    return (result);
}
