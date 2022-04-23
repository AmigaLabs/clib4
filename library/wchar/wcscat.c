/*
 * $Id: wchar_wcscat.c,v 1.3 2006-01-08 12:04:27 clib2devs Exp $
*/

#ifndef _WCHAR_HEADERS_H
#include "wchar_headers.h"
#endif /* _WCHAR_HEADERS_H */

wchar_t *
wcscat(wchar_t *dest, const wchar_t *src) {
    wcscpy(dest + wcslen(dest), src);
    return dest;
}
