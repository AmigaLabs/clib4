/*
 * $Id: wchar_wcsncasecmp_l.c,v 1.0 2022-08-04 12:04:27 clib4devs Exp $
*/

#ifndef _WCHAR_HEADERS_H
#include "wchar_headers.h"
#endif /* _WCHAR_HEADERS_H */

int
wcsncasecmp_l(const wchar_t *l, const wchar_t *r, size_t n, locale_t locale) {
    (void) locale;

    return wcsncasecmp(l, r, n);
}
