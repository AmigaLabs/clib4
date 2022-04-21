/*
 * $Id: wchar_swscanf.c,v 1.3 2006-01-08 12:04:27 clib2devs Exp $
*/

#ifndef _WCHAR_HEADERS_H
#include "wchar_headers.h"
#endif /* _WCHAR_HEADERS_H */

int
swscanf(const wchar_t *s, const wchar_t *format, ...) {
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
    result = vswscanf(s, format, ap);
    va_end(ap);

out:
    RETURN(result);
    return result;
}
