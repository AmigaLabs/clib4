/*
* $Id: ctype_ispunct.c,v 1.6 2025-06-30 12:04:22 clib4devs Exp $
*/

#include <ctype.h>

#undef ispunct

int
ispunct(int c) {
	return ((_ctype_ + 1)[c] & _P);
}
