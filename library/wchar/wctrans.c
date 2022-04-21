/*
 * $Id: wctype_wctrans.c,v 1.4 2021-02-04 00:26:27 clib2devs Exp $
*/

#ifndef _WCHAR_HEADERS_H
#include "wchar_headers.h"
#endif /* _WCHAR_HEADERS_H */

#ifndef _WCTYPE_HEADERS_H
#include <wctype.h>
#endif /* _WCTYPE_HEADERS_H */

wctrans_t
wctrans(const char *property)
{
    ENTER();

    if (!strcmp(property, "tolower"))
		return WCT_TOLOWER;
	else if (!strcmp(property, "toupper"))
		return WCT_TOUPPER;
	else
	{
		__set_errno(EINVAL);
		return 0;
	}
}
