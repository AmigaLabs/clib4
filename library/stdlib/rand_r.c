/*
 * $Id: stdlib_rand_r.c,v 1.2 2006-01-08 12:04:26 clib2devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

/****************************************************************************/

/* Parameters of a pseudo-random-number generator from Knuth's
   "The Art of Computer Programming, Volume 2: Seminumerical Algorithms"
   (3rd edition), pp. 185-186. */

#define MM 2147483647	/* a Mersenne prime */
#define AA 48271		/* this does well in the spectral test */
#define QQ 44488		/* (long)(MM/AA) */
#define RR 3399			/* MM % AA; it is important that RR < QQ */

/****************************************************************************/

int
rand_r(unsigned int * seed)
{
	int X;

	X = (int)((*seed) & 0x7fffffff);
	if(X == 0)
		X = 1; /* NOTE: for Knuth's algorithm the seed must not be zero. */

	X = AA * (X % QQ) - RR * (long)(X / QQ);
	if(X < 0)
		X += MM;

	(*seed) = (unsigned int)X;

	return(X);
}
