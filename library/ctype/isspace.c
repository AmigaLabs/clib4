/*
* $Id: ctype_isspace.c,v 1.6 2025-06-30 12:04:22 clib4devs Exp $
*/

#include <ctype.h>

#undef isspace

int
isspace(int c) {
	return ((_ctype_ + 1)[c] & _S);
}
