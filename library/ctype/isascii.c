/*
* $Id: ctype_isascii.c,v 1.6 2025-06-30 12:04:22 clib4devs Exp $
*/

#include <ctype.h>

#undef isascii

int
isascii(int c) {
	return c >= 0 && c < 128;
}
