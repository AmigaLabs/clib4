/*
 * $Id: wctype_iswspace.c,v 1.4 2021-02-03 00:15:27 clib2devs Exp $
*/

#ifndef _WCHAR_HEADERS_H
#include "wchar_headers.h"
#endif /* _WCHAR_HEADERS_H */

#ifndef _WCTYPE_HEADERS_H
#include <wctype.h>
#endif /* _WCTYPE_HEADERS_H */

#include <ctype.h>

int iswspace(wint_t c)
{
	int unicode = 0;
	if (__global_clib2->_current_locale[0] == 'C' && __global_clib2->_current_locale[1] == '\0')
	{
		unicode = 0;
		/* fall-through */
	}
	else if (!strcmp(__global_clib2->_current_locale, "C-JIS"))
	{
		c = __jp2uc(c, JP_JIS);
		unicode = 1;
	}
	else if (!strcmp(__global_clib2->_current_locale, "C-SJIS"))
	{
		c = __jp2uc(c, JP_SJIS);
		unicode = 1;
	}
	else if (!strcmp(__global_clib2->_current_locale, "C-EUCJP"))
	{
		c = __jp2uc(c, JP_EUCJP);
		unicode = 1;
	}
	else if (!strcmp(__global_clib2->_current_locale, "C-UTF-8"))
	{
		unicode = 1;
	}

	if (unicode)
	{
		return ((c >= 0x0009 && c <= 0x000d) || c == 0x0020 || c == 0x1680 ||
				(c >= 0x2000 && c <= 0x2006) ||
				(c >= 0x2008 && c <= 0x200b) ||
				c == 0x2028 || c == 0x2029 ||
				c == 0x205f || c == 0x3000);
	}

	return (c < 0x100 ? isspace(c) : 0);
}
