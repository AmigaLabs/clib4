/*
* $Id: ctype_isblank.c,v 1.6 2025-06-30 12:04:22 clib4devs Exp $
*/

#include <ctype.h>

#undef isblank

int
isblank(int c) {
	return (c == ' ' || c == '\t');
}
