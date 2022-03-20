/*
 * $Id: math_fmaf.c,v 1.3 2006-01-08 12:04:23 clib2devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

float 
fmaf(float x, float y, float z)
{
	/* ZZZ this should be a *fused* multiply & add, and
	   not a sequential operation as declared below! */
	return ((x * y) + z);
}
