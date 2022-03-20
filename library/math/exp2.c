/*
 * $Id: math_exp2.c,v 1.3 2006-01-08 12:04:23 clib2devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

double
exp2(double x)
{
	return (pow(2.0, x));
}
