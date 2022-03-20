/*
 * $Id: wctype_towctrans.c,v 1.4 2021-02-04 00:20:27 clib2devs Exp $
*/

#ifndef _WCHAR_HEADERS_H
#include "wchar_headers.h"
#endif /* _WCHAR_HEADERS_H */

#ifndef _WCTYPE_HEADERS_H
#include <wctype.h>
#endif /* _WCTYPE_HEADERS_H */

wint_t
towctrans(wint_t c, wctrans_t desc)
{
	if (desc == WCT_TOLOWER)
		return towlower(c);
	else if (desc == WCT_TOUPPER)
		return towupper(c);
	else
	{
		__set_errno(EINVAL);
		return c;
	}
}