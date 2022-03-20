/*
 * $Id: stdlib_div.c,v 1.3 2006-01-08 12:04:25 clib2devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

/****************************************************************************/

div_t
div(int n,int d)
{
	div_t result;

	assert( d != 0 );

	result.quot	= n / d;
	result.rem	= n % d;

	return(result);
}
