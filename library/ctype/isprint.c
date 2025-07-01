/*
* $Id: ctype_isprint.c,v 1.6 2025-06-30 12:04:22 clib4devs Exp $
*/

#ifndef _CTYPE_HEADERS_H
#include "ctype_headers.h"
#endif /* _CTYPE_HEADERS_H */

#undef isprint

int
isprint(int c) {
	return ((_ctype_ + 1)[c] & (_P | _U | _L | _N | _B));
}