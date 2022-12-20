/*
 * $Id: 
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

int
dprintf(int fd, const char *format, ...) {
    int result = EOF;
    va_list arg;

    ENTER();

    SHOWVALUE(fd);
    SHOWSTRING(format);

    assert(format != NULL);

    if (format == NULL) {
        __set_errno(EFAULT);
        goto out;
    }

    va_start(arg, format);
    result = vdprintf(fd, format, arg);
    va_end(arg);

out:

    RETURN(result);
    return (result);
}
