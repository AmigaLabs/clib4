/*
 * $Id: math_copysign.c,v 1.2 2006-01-08 12:04:23 clib2devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

double
copysign(double x, double y)
{
    uint32_t hx, hy;

	GET_HIGH_WORD(hx, x);
	GET_HIGH_WORD(hy, y);
	SET_HIGH_WORD(x, (hx & 0x7fffffff) | (hy & 0x80000000U));

	return x;
}
