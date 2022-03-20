/*
 * $Id: math_fmin.c,v 1.3 2006-01-08 12:04:23 clib2devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

double
fmin(double x, double y)
{
	double result;

	if (isnan(x))
	{
		if (isnan(y))
			result = nan(NULL);
		else
			result = y;
	}
	else if (isnan(y))
	{
		result = x;
	}
	else
	{
		if (x < y)
			result = x;
		else
			result = y;
	}

	return (result);
}
