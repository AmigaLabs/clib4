/*
 * $Id: wctype_iswupper.c,v 1.4 2021-02-04 00:16:27 clib2devs Exp $
*/

#ifndef _WCTYPE_HEADERS_H
#include <wctype.h>
#endif /* _WCTYPE_HEADERS_H */

#include <ctype.h>

int
iswupper(wint_t c)
{
	return (towlower (c) != c);
}
