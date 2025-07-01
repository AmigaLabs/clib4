/*
* $Id: ctype_isalpha.c,v 1.6 2025-06-30 12:04:22 clib4devs Exp $
*/

#ifndef _CTYPE_HEADERS_H
#include "ctype_headers.h"
#endif /* _CTYPE_HEADERS_H */

#undef isalpha

int
isalpha (int c) {
	return ((_ctype_ + 1)[c] & (_U | _L));
}