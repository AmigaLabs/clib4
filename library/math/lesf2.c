/*
 * $Id: math_lesf2.c,v 1.3 2006-01-08 12:04:23 clib2devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

/****************************************************************************/

#if defined(IEEE_FLOATING_POINT_SUPPORT) && defined(__GNUC__)

/****************************************************************************/

float
__lesf2(float x,float y)
{
	float result;

	result = IEEESPCmp(x,y);

	return(result);
}

/****************************************************************************/

#endif /* IEEE_FLOATING_POINT_SUPPORT */
