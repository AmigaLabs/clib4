/*
 * $Id: math_nearbyintf.c,v 1.3 2006-01-08 12:04:24 clib2devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

float 
nearbyintf(float x)
{
	/* ZZZ is this such a good idea? */
	return (rintf(x));
}
