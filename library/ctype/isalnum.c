/*
* $Id: ctype_isalnum.c,v 1.6 2025-06-30 12:04:22 clib4devs Exp $
*/

#include <ctype.h>

#undef isalnum

int
isalnum(int c) {
	return ((_ctype_ + 1)[c] & (_U | _L | _N));
}
