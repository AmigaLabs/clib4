/*
 * $Id: math_e_cosh.c,v 1.8 2023-07-19 12:04:23 clib2devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

static const double one = 1.0, half = 0.5, huge = 1.0e300;

double
cosh(double x) {
    double t, w;
    int32_t ix;

    /* High word of |x|. */
    GET_HIGH_WORD(ix, x);
    ix &= 0x7fffffff;

    /* x is INF or NaN */
    if (ix >= 0x7ff00000) return x * x;

    /* |x| in [0,0.5*ln2], return 1+expm1(|x|)^2/(2*exp(|x|)) */
    if (ix < 0x3fd62e43) {
        t = expm1(fabs(x));
        w = one + t;
        if (ix < 0x3c800000) return w;    /* cosh(tiny) = 1 */
        return one + (t * t) / (w + w);
    }

    /* |x| in [0.5*ln2,22], return (exp(|x|)+1/exp(|x|)/2; */
    if (ix < 0x40360000) {
        t = __ieee754_exp(fabs(x));
        return half * t + half / t;
    }

    /* |x| in [22, log(maxdouble)] return half*exp(|x|) */
    if (ix < 0x40862E42) return half * __ieee754_exp(fabs(x));

    /* |x| in [log(maxdouble), overflowthresold] */
    if (ix <= 0x408633CE)
        return __ldexp_exp(fabs(x), -1);

    /* |x| > overflowthresold, cosh(x) overflow */
    return huge * huge;
}
