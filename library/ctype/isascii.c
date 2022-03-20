/*
 * $Id: ctype_isascii.c,v 1.4 2021-02-10 23:46:22 clib2devs Exp $
*/

#ifndef _CTYPE_HEADERS_H
#include "ctype_headers.h"
#endif /* _CTYPE_HEADERS_H */

int 
isascii(int c)
{
	return c >= 0 && c < 128;
}
