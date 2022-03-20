/*
 * $Id: math_atanhf.c,v 1.3 2006-01-08 12:04:23 clib2devs Exp $
 *

 *
 *
 * PowerPC math library based in part on work by Sun Microsystems
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunPro, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice
 * is preserved.
 *
 * Conversion to float by Ian Lance Taylor, Cygnus Support, ian@cygnus.com.
 */

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

static const float zero = 0.0;
static const float one = 1.0, huge = 1e30;

float
atanhf(float x)
{
	float t;
	LONG hx, ix;
	GET_FLOAT_WORD(hx, x);
	ix = hx & 0x7fffffff;
	if (ix > 0x3f800000) /* |x|>1 */
		return (x - x) / (x - x);
	if (ix == 0x3f800000)
		return x / zero;
	if (ix < 0x31800000 && (huge + x) > zero)
		return x; /* x<2**-28 */
	SET_FLOAT_WORD(x, ix);
	if (ix < 0x3f000000)
	{ /* x < 0.5 */
		t = x + x;
		t = (float)0.5 * log1pf(t + t * x / (one - x));
	}
	else
		t = (float)0.5 * log1pf((x + x) / (one - x));
	if (hx >= 0)
		return t;
	else
		return -t;
}
