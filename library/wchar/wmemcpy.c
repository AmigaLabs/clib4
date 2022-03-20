/*
 * $Id: wchar_wmemcpy.c,v 1.4 2021-02-04 11:14:27 clib2devs Exp $
*/

#ifndef _WCHAR_HEADERS_H
#include "wchar_headers.h"
#endif /* _WCHAR_HEADERS_H */

wchar_t *
wmemcpy(wchar_t *dest, const wchar_t *src, size_t len)
{
	return (wchar_t *)memcpy(dest, src, len * sizeof(wchar_t));
}
