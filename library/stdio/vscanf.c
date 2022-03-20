/*
 * $Id: stdio_vscanf.c,v 1.2 2006-01-08 12:04:25 clib2devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

int
vscanf(const char *format, va_list arg) {
    int result = EOF;

    ENTER();

    SHOWSTRING(format);

    assert(format != NULL);

    if (__check_abort_enabled)
        __check_abort();

    if (format == NULL) {
        SHOWMSG("invalid format parameter");

        __set_errno(EFAULT);
        goto out;
    }

    result = vfscanf(stdin, format, arg);

out:

    RETURN(result);
    return (result);
}
