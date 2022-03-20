/*
 * $Id: math_fabs.c,v 1.9 2021-01-31 12:04:23 clib2devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

INLINE STATIC double
__fabs(double x)
{
	double res;

	__asm volatile("fabs %0, %1"
				   : "=f"(res)
				   : "f"(x));

	return res;
}

double
fabs(double x)
{
	double result;

	result = __fabs(x);

	return (result);
}
