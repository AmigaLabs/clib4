/*
 * $Id: wchar_wcsspn.c,v 1.3 2006-01-08 12:04:27 clib4devs Exp $
*/

#ifndef _WCHAR_HEADERS_H
#include "wchar_headers.h"
#endif /* _WCHAR_HEADERS_H */

size_t
wcscspn(const wchar_t *s, const wchar_t *c) {
    const wchar_t *a;

    if (!c[0]) {
        return wcslen(s);
    }
    if (!c[1]) {
        return (s = wcschr(a = s, *c)) ? (size_t)(s - a) : wcslen(a);
    }

    ENTER();

    for (a = s; *s && !wcschr(c, *s); s++);

    size_t result = s - a;

    RETURN(result);
    return result;
}
