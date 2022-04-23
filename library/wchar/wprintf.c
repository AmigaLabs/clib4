/*
 * $Id: wchar_wprintf.c,v 1.4 2021-02-05 00:42:44 clib2devs Exp $
*/

#ifndef _WCHAR_HEADERS_H
#include "wchar_headers.h"
#endif /* _WCHAR_HEADERS_H */

int
wprintf(const wchar_t *format, ...) {
    int result = EOF;

    ENTER();

    SHOWWSTRING(format);

    assert(format != NULL);

    if (__check_abort_enabled)
        __check_abort();

    if (format == NULL) {
        __set_errno(EFAULT);
        goto out;
    }

    va_list ap;
    va_start(ap, format);
    result = vfwprintf(stdout, format, ap);
    va_end(ap);

out:
    RETURN(result);
    return (result);
}
