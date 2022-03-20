/*
 * $Id: math_ldexp.c,v 1.6 2006-01-08 12:04:23 clib2devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

double
ldexp(double x, int exp)
{
	double result;

	if (isinf(x) || fpclassify(x) == FP_ZERO)
	{
		result = x;
	}
	else
	{
		result = scalbn(x, exp);

		if (isinf(result) || (result < DBL_MIN || result > -DBL_MIN))
			__set_errno(ERANGE);
	}

	return (result);
}
