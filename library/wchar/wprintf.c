/*
 * $Id: wchar_wprintf.c,v 1.4 2021-02-05 00:42:44 clib4devs Exp $
*/

#ifndef _WCHAR_HEADERS_H
#include "wchar_headers.h"
#endif /* _WCHAR_HEADERS_H */

#include "stdio_headers.h"

int
wprintf(const wchar_t *format, ...) {
    int result = EOF;
    struct _clib4 *__clib4 = __CLIB4;

    ENTER();

    SHOWWSTRING(format);

    assert(format != NULL);

    if (format == NULL) {
        __set_errno_r(__clib4, EFAULT);
        goto out;
    }

    va_list ap;
    va_start(ap, format);
    result = vfwprintf(__stdout_r(__clib4), format, ap);
    va_end(ap);

out:
    RETURN(result);
    return (result);
}
