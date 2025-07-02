/*
* $Id: ctype_isgraph.c,v 1.6 2025-06-30 12:04:22 clib4devs Exp $
*/

#include <ctype.h>

#undef isgraph

int
isgraph(int c) {
	return ((_ctype_ + 1)[c] & (_P | _U | _L | _N));
}
