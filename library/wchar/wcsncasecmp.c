/*
 * $Id: wchar_wcsncasecmp.c,v 1.0 2022-08-04 12:04:27 clib4devs Exp $
*/

#ifndef _WCHAR_HEADERS_H
#include "wchar_headers.h"
#endif /* _WCHAR_HEADERS_H */

#ifndef _WCTYPE_HEADERS_H
#include <wctype.h>
#endif /* _WCTYPE_HEADERS_H */

int
wcsncasecmp(const wchar_t *l, const wchar_t *r, size_t n) {
    if (!n--) return 0;
    for (; *l && *r && n && (*l == *r || towlower(*l) == towlower(*r)); l++, r++, n--);
    return towlower(*l) - towlower(*r);
}
