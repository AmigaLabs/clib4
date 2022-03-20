/*
 * $Id: inttypes_imaxdiv.c,v 1.2 2006-01-08 12:04:22 clib2devs Exp $
*/

#ifndef _INTTYPES_HEADERS_H
#include "inttypes_headers.h"
#endif /* _INTTYPES_HEADERS_H */

/****************************************************************************/

imaxdiv_t
imaxdiv(intmax_t n,intmax_t d)
{
	imaxdiv_t result;

	assert( d != 0 );

	result.quot	= n / d;
	result.rem	= n % d;

	return(result);
}
