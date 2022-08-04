/*
 * $Id: wchar_wcsnlen.c,v 1.0 2022-08-04 12:04:27 clib2devs Exp $
*/

#ifndef _WCHAR_HEADERS_H
#include "wchar_headers.h"
#endif /* _WCHAR_HEADERS_H */

size_t
wcsnlen(const wchar_t *s, size_t n) {
    const wchar_t *z = wmemchr(s, 0, n);
    if (z) n = z - s;
    return n;
}
