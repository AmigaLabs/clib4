/*
 * $Id: wchar_wcsdup.c,v 1.0 2022-08-04 12:04:27 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _WCHAR_HEADERS_H
#include "wchar_headers.h"
#endif /* _WCHAR_HEADERS_H */

wchar_t *wcsdup(const wchar_t *s) {
    size_t l = wcslen(s);
    wchar_t *d = malloc((l + 1) * sizeof(wchar_t));
    if (!d) return NULL;
    return wmemcpy(d, s, l + 1);
}
