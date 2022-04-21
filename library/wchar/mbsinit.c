/*
 * $Id: wchar_mbsinit.c,v 1.3 2006-01-08 12:04:27 clib2devs Exp $
*/

#ifndef _WCHAR_HEADERS_H
#include "wchar_headers.h"
#endif /* _WCHAR_HEADERS_H */

/****************************************************************************/

int
mbsinit(const mbstate_t *ps)
{
    ENTER();

	return !ps || !*(unsigned *)ps;
}
