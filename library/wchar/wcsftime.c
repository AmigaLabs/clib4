/*
 * $Id: wchar_wcsftime.c,v 1.3 2006-01-08 12:04:27 clib2devs Exp $
*/

#ifndef _WCHAR_HEADERS_H
#include "wchar_headers.h"
#endif /* _WCHAR_HEADERS_H */

#include "wchar_wprintf_core.h"

size_t
wcsftime(wchar_t *w, size_t sz, const wchar_t *fmt, const struct tm *v)
{
	size_t bsz = ((sz + 1) * sizeof(wchar_t));
	const struct tm *ptm = (const struct tm *)v;

    ENTER();

    if ((!w) || (!fmt) || (!sz) || (!v))
	{
		__set_errno(EINVAL);
		return 0U;
	}

	do
	{
		char fb[bsz];
		char cb[sz];
		size_t osz = 0U;
		__set_errno(0);

		if (
			(!wstring_wstocs(fb, bsz, fmt, 0)) ||
			(!(osz = strftime(cb, sz, fb, ptm))) ||
			(!(osz = wstring_cstows(w, sz, cb, osz))))
		{
			break;
		}

		return osz;

	} while (0);

	return 0U;
}
