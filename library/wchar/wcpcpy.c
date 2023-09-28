/*
 * $Id: wchar_wcpcpy.c,v 1.0 2022-08-04 12:04:27 clib4devs Exp $
*/

#ifndef _WCHAR_HEADERS_H
#include "wchar_headers.h"
#endif /* _WCHAR_HEADERS_H */

wchar_t *
wcpcpy(wchar_t *d, const wchar_t *s) {
    return wcscpy(d, s) + wcslen(s);
}
