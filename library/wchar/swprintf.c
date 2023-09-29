/*
 * $Id: wchar_swprintf.c,v 1.3 2006-01-08 12:04:27 clib4devs Exp $
*/

#ifndef _WCHAR_HEADERS_H
#include "wchar_headers.h"
#endif /* _WCHAR_HEADERS_H */

/****************************************************************************/

int
swprintf(wchar_t *restrict s, size_t l, const wchar_t *restrict fmt, ...)
{
    int ret;
    va_list ap;

    ENTER();

    va_start(ap, fmt);
    ret = vswprintf(s, l, fmt, ap);
    va_end(ap);
    return ret;
}
