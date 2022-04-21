/*
 * $Id: wchar_wcspbrk.c,v 1.3 2006-01-08 12:04:27 clib2devs Exp $
*/

#ifndef _WCHAR_HEADERS_H
#include "wchar_headers.h"
#endif /* _WCHAR_HEADERS_H */

#include "wchar_wprintf_core.h"

wchar_t *
wcspbrk(const wchar_t *s, const wchar_t *set)
{
	const wchar_t *p;
	const wchar_t *q;

    ENTER();

    p = s;
	while (*p)
	{
		q = set;
		while (*q)
		{
			if (*p == *q)
			{
				/* LINTED interface specification */
				return (wchar_t *)p;
			}
			q++;
		}
		p++;
	}
	return NULL;
}
