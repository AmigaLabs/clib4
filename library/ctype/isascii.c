/*
* $Id: ctype_isascii.c,v 1.6 2025-06-30 12:04:22 clib4devs Exp $
*/

#ifndef _CTYPE_HEADERS_H
#include "ctype_headers.h"
#endif /* _CTYPE_HEADERS_H */

#undef isascii

int
isascii (int c) {
	return c >= 0 && c < 128;
}