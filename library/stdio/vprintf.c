/*
 * $Id: stdio_vprintf.c,v 1.6 2006-01-08 12:04:25 clib4devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

int
vprintf(const char *format, va_list arg) {
    int result = EOF;
    struct _clib4 *__clib4 = __CLIB4;

    ENTER();

    SHOWSTRING(format);
    SHOWPOINTER(arg);

#if defined(__arm__)
    assert(format != NULL);
#else
    assert(format != NULL && arg != NULL);
#endif

    if (format == NULL || VA_LIST_IS_NULL(arg)) {
        __set_errno_r(__clib4, EFAULT);
        goto out;
    }

    __flockfile_r(__clib4, __stdout_r(__clib4));
    result = __vfprintf_r(__clib4, __stdout_r(__clib4), format, arg);
    __funlockfile_r(__clib4, __stdout_r(__clib4));

out:

    RETURN(result);
    return (result);
}
