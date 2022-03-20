/*
 * $Id: stdlib_lldiv.c,v 1.2 2006-01-08 12:04:25 clib2devs Exp $
*/

#undef __STRICT_ANSI__

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

/****************************************************************************/

#if defined(__GNUC__) || (defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L))

/****************************************************************************/

lldiv_t
lldiv(long long n,long long d)
{
	lldiv_t result;

	assert( d != 0 );

	result.quot	= n / d;
	result.rem	= n % d;

	return(result);
}

/****************************************************************************/

#endif
