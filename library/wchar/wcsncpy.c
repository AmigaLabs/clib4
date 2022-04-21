/*
 * $Id: wchar_wcsncpy.c,v 1.3 2006-01-08 12:04:27 clib2devs Exp $
*/

#ifndef _WCHAR_HEADERS_H
#include "wchar_headers.h"
#endif /* _WCHAR_HEADERS_H */

wchar_t *
wcsncpy(wchar_t *dest, const wchar_t *src, size_t n)
{
    ENTER();

    wchar_t *a = dest;
	while (n && *src) n--, *dest++ = *src++;
	wmemset(dest, 0, n);
	return a;
}
