/*
 * $Id: math_exp2f.c,v 1.3 2006-01-08 12:04:23 clib2devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

float 
exp2f(float x)
{
	return (powf(2.0, x));
}
