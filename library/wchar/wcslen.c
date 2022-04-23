/*
 * $Id: wchar_wcslen.c,v 1.3 2006-01-08 12:04:27 clib2devs Exp $
*/

#ifndef _WCHAR_HEADERS_H
#include "wchar_headers.h"
#endif /* _WCHAR_HEADERS_H */

size_t
wcslen(const wchar_t *s) {
    const wchar_t *p;

    ENTER();

    p = s;
    while (*p)
        p++;

    size_t result = p - s;

    RETURN(result);
    return result;
}
