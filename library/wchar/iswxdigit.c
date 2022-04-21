/*
 * $Id: wctype_iswxdigit.c,v 1.4 2021-02-04 00:17:27 clib2devs Exp $
*/

#ifndef _WCTYPE_HEADERS_H
#include <wctype.h>
#endif /* _WCTYPE_HEADERS_H */

#include <ctype.h>

int 
iswxdigit(wint_t c)
{
    ENTER();

	return ((c >= (wint_t)'0' && c <= (wint_t)'9') ||
			(c >= (wint_t)'a' && c <= (wint_t)'f') ||
			(c >= (wint_t)'A' && c <= (wint_t)'F'));
}
