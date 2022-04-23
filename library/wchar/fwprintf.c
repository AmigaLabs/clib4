/*
 * $Id: wchar_fwprintf.c,v 1.1 2022-03-04 16:32:11 clib2devs Exp $
*/

#ifndef _WCHAR_HEADERS_H
#include "wchar_headers.h"
#endif /* _WCHAR_HEADERS_H */

int
fwprintf(FILE *f, const wchar_t *fmt, ...) {
    int ret;

    ENTER();

    va_list ap;
    va_start(ap, fmt);
    ret = vfwprintf(f, fmt, ap);
    va_end(ap);

    RETURN(ret);
    return ret;
}
