/*
 * $Id: wctype_iswgraph.c,v 1.4 2021-02-04 00:01:27 clib2devs Exp $
*/

#ifndef _WCTYPE_HEADERS_H
#include <wctype.h>
#endif /* _WCTYPE_HEADERS_H */

#include <ctype.h>

/****************************************************************************/

int
iswgraph(wint_t c)
{
	return (iswprint (c) && !iswspace (c));
}
