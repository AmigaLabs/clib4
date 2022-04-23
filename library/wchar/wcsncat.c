/*
 * $Id: wchar_wcsncat.c,v 1.3 2006-01-08 12:04:27 clib2devs Exp $
*/

#ifndef _WCHAR_HEADERS_H
#include "wchar_headers.h"
#endif /* _WCHAR_HEADERS_H */

wchar_t *
wcsncat(wchar_t *dest, const wchar_t *src, size_t n)
{
    ENTER();

    wchar_t *a = dest;
	dest += wcslen(dest);
	while (n && *src) n--, *dest++ = *src++;
	*dest++ = 0;

    LEAVE();
	return a;
}
