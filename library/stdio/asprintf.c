/*
 * $Id: stdio_asprintf.c,v 1.7 2006-01-08 12:04:24 clib2devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

int
asprintf(char **ret, const char *format, ...) {
    int result = EOF;
    va_list arg;

    ENTER();

    SHOWPOINTER(ret);
    SHOWSTRING(format);

    assert(ret != NULL && format != NULL);

    if (ret == NULL || format == NULL) {
        __set_errno(EFAULT);
        goto out;
    }

    va_start(arg, format);
    result = vasprintf(ret, format, arg);
    va_end(arg);

out:

    RETURN(result);
    return (result);
}
