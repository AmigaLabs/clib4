/*
 * $Id: wchar_wcscasecmp_l.c,v 1.0 2022-08-04 12:04:27 clib4devs Exp $
*/

#ifndef _WCHAR_HEADERS_H
#include "wchar_headers.h"
#endif /* _WCHAR_HEADERS_H */

int
wcscasecmp_l(const wchar_t *l, const wchar_t *r, locale_t locale) {
    (void) locale;

    return wcscasecmp(l, r);
}
