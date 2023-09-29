/*
 * $Id: wchar_wcscasecmp.c,v 1.0 2022-08-04 12:04:27 clib4devs Exp $
*/

#ifndef _WCHAR_HEADERS_H
#include "wchar_headers.h"
#endif /* _WCHAR_HEADERS_H */

#ifndef _WCTYPE_HEADERS_H
#include <wctype.h>
#endif /* _WCTYPE_HEADERS_H */

int
wcscasecmp(const wchar_t *l, const wchar_t *r) {
    return wcsncasecmp(l, r, -1);
}
