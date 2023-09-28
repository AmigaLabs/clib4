/*
 * $Id: wchar_wcsspn.c,v 1.3 2006-01-08 12:04:27 clib4devs Exp $
*/

#ifndef _WCHAR_HEADERS_H
#include "wchar_headers.h"
#endif /* _WCHAR_HEADERS_H */

#include "wchar_wprintf_core.h"

size_t
wcsspn(const wchar_t *s, const wchar_t *set) {
    const wchar_t *p;
    const wchar_t *q;
    size_t result;

    ENTER();

    p = s;
    while (*p) {
        q = set;
        while (*q) {
            if (*p == *q)
                break;
            q++;
        }
        if (!*q)
            goto done;
        p++;
    }

done:

    result = p - s;
    RETURN(result);
    return result;
}
