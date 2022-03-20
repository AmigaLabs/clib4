/*
 * $Id: math_sin.c,v 1.6 2021-01-31 12:04:24 clib2devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

INLINE STATIC double
__sin(double x)
{
	double y[2], z = 0.0;
	int n, ix;

	/* High word of x. */
	GET_HIGH_WORD(ix, x);

	/* |x| ~< pi/4 */
	ix &= 0x7fffffff;
	if (ix <= 0x3fe921fb)
		return __kernel_sin(x, z, 0);

	/* sin(Inf or NaN) is NaN */
	else if (ix >= 0x7ff00000)
		return x - x;

	/* argument reduction needed */
	else
	{
		n = __rem_pio2(x, y);
		switch (n & 3)
		{
		case 0:
			return __kernel_sin(y[0], y[1], 1);
		case 1:
			return __kernel_cos(y[0], y[1]);
		case 2:
			return -__kernel_sin(y[0], y[1], 1);
		default:
			return -__kernel_cos(y[0], y[1]);
		}
	}
}

double
sin(double x)
{
	double result;

	result = __sin(x);

	return (result);
}
