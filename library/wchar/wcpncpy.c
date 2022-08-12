/*
 * $Id: wchar_wcpncpy.c,v 1.0 2022-08-04 12:04:27 clib2devs Exp $
*/

#ifndef _WCHAR_HEADERS_H
#include "wchar_headers.h"
#endif /* _WCHAR_HEADERS_H */

wchar_t *
wcpncpy(wchar_t *d, const wchar_t *s, size_t n) {
    return wcsncpy(d, s, n) + wcsnlen(s, n);
}
