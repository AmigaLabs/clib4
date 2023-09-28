/*
 * $Id: wchar_wcsncmp.c,v 1.3 2006-01-08 12:04:27 clib4devs Exp $
*/

#ifndef _WCHAR_HEADERS_H
#include "wchar_headers.h"
#endif /* _WCHAR_HEADERS_H */

int
wcsncmp(const wchar_t *s1, const wchar_t *s2, size_t n) {
    ENTER();

    for (; n && *s1 == *s2 && *s1 && *s2; n--, s1++, s2++);

    RETURN(n ? *s1 - *s2 : 0);
    return n ? *s1 - *s2 : 0;
}
