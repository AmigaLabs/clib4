/*
 * $Id: math_fdim.c,v 1.3 2006-01-08 12:04:23 clib2devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

double
fdim(double x, double y)
{
	double result;

    if (isnan(x))
        return x;

    if (isnan(y))
        return y;

    if (x > y)
		result = x - y;
	else
		result = 0;

	return (result);
}
