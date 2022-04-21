/*
 * $Id: wchar_wcscmp.c,v 1.4 2021-02-04 00:30:27 clib2devs Exp $
*/

#ifndef _WCHAR_HEADERS_H
#include "wchar_headers.h"
#endif /* _WCHAR_HEADERS_H */

int
wcscmp(const wchar_t *s1, const wchar_t *s2) {
    wchar_t c1, c2;

    ENTER();

    if ((!s1) || (!s2)) {
        return -1;
    }

    do {
        c1 = *s1++;
        c2 = *s2++;
        if (c2 == L'\0')
            return c1 - c2;

    } while (c1 == c2);

    return c1 < c2 ? -1 : 1;
}
