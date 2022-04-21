/*
 * $Id: wctype_iswdigit.c,v 1.4 2021-02-04 00:00:27 clib2devs Exp $
*/

#ifndef _WCTYPE_HEADERS_H
#include <wctype.h>
#endif /* _WCTYPE_HEADERS_H */

#include <ctype.h>

int
iswdigit(wint_t c)
{
    ENTER();

	return (unsigned)c - '0' < 10;
}
