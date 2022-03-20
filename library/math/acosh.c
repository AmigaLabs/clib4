/*
 * $Id: math_acosh.c,v 1.3 2006-01-08 12:04:23 clib2devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

static const double
	one = 1.0,
	ln2 = 6.93147180559945286227e-01; /* 0x3FE62E42, 0xFEFA39EF */

INLINE STATIC double
__acosh(double x)
{
	double t;
	LONG hx;
	ULONG lx;
	EXTRACT_WORDS(hx, lx, x);
	if (hx < 0x3ff00000)
	{ /* x < 1 */
		return (x - x) / (x - x);
	}
	else if (hx >= 0x41b00000)
	{ /* x > 2**28 */
		if (hx >= 0x7ff00000)
		{ /* x is inf of NaN */
			return x + x;
		}
		else
			return log(x) + ln2; /* acosh(huge)=log(2x) */
	}
	else if (((hx - 0x3ff00000) | lx) == 0)
	{
		return 0.0; /* acosh(1) = 0 */
	}
	else if (hx > 0x40000000)
	{ /* 2**28 > x > 2 */
		t = x * x;
		return log(2.0 * x - one / (x + sqrt(t - one)));
	}
	else
	{ /* 1<x<2 */
		t = x - one;
		return log1p(t + sqrt(2.0 * t + t * t));
	}
}

double
acosh(double x) {
    double result;

    if (x < 1.0)
    {
        result = NAN;
        __set_errno(EDOM);
        feraiseexcept(FE_INVALID);
    }
    else
    {
        result = __acosh(x);
    }

    return (result);
}