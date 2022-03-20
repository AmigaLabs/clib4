/*
 * $Id: stdlib_mbstowcs.c,v 1.3 2006-01-08 12:04:26 clib2devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

/****************************************************************************/

size_t
mbstowcs(wchar_t *ws, const char *s, size_t wn)
{
	return mbsrtowcs(ws, (void*)&s, wn, 0);
}
