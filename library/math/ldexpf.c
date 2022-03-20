/*
 * $Id: math_ldexpf.c,v 1.3 2006-01-08 12:04:23 clib2devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

float ldexpf(float x, int exp)
{
	float result;

	if (isinf(x) || fpclassify(x) == FP_ZERO)
	{
		result = x;
	}
	else
	{
		result = scalbnf(x, exp);

		if (isinf(result) || (result < FLT_MIN || result > -FLT_MIN))
			__set_errno(ERANGE);
	}

	return (result);
}
