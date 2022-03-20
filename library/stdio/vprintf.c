/*
 * $Id: stdio_vprintf.c,v 1.6 2006-01-08 12:04:25 clib2devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

int
vprintf(const char *format, va_list arg) {
    int result = EOF;

    ENTER();

    SHOWSTRING(format);
    SHOWPOINTER(arg);

    assert(format != NULL && arg != NULL);

    if (__check_abort_enabled)
        __check_abort();

    if (format == NULL || arg == NULL) {
        __set_errno(EFAULT);
        goto out;
    }

    result = vfprintf(stdout, format, arg);

out:

    RETURN(result);
    return (result);
}
