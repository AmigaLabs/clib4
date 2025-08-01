/*
 * $Id: stdio_vscanf.c,v 1.2 2006-01-08 12:04:25 clib4devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

int
vscanf(const char *format, va_list arg) {
    int result = EOF;
    struct _clib4 *__clib4 = __CLIB4;

    ENTER();

    SHOWSTRING(format);

    assert(format != NULL);

    if (format == NULL) {
        SHOWMSG("invalid format parameter");

        __set_errno_r(__clib4, EFAULT);
        goto out;
    }

    result = vfscanf(__stdin_r(__clib4), format, arg);

out:

    RETURN(result);
    return (result);
}
