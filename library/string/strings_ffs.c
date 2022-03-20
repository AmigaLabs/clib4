/*
 * $Id: strings_ffs.c,v 1.2 2006-01-08 12:04:27 clib2devs Exp $
*/

#ifndef _STRINGS_HEADERS_H
#include "strings_headers.h"
#endif /* _STRINGS_HEADERS_H */

/* Return the index of the lowest set bit. (Counted from one) */
int
ffs(int i)
{
	int result = 0;

	if(i != 0)
	{
		int x;

		x = (i & (-i)) - 1;
		x -= ((x >> 1) & 0x55555555);
		x = ((x >> 2) & 0x33333333) + (x & 0x33333333);
		x = ((x >> 4) + x) & 0x0f0f0f0f;
		x += (x >> 8);
		x += (x >> 16);

		result = 1 + (x & 0x3f); /* The first bit has index 1. */
	}

	return(result);
}
