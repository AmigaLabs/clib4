/*
 * $Id: math_e_coshf.c,v 1.5 2023-07-19 12:04:23 clib2devs Exp $
 */

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

static const float one = 1.0, half = 0.5, huge = 1.0e30;

float
coshf(float x) {
    float t, w;
    int32_t ix;

    GET_FLOAT_WORD(ix, x);
    ix &= 0x7fffffff;

    /* x is INF or NaN */
    if (ix >= 0x7f800000) return x * x;

    /* |x| in [0,0.5*ln2], return 1+expm1(|x|)^2/(2*exp(|x|)) */
    if (ix < 0x3eb17218) {
        t = expm1f(fabsf(x));
        w = one + t;
        if (ix < 0x39800000) return one;    /* cosh(tiny) = 1 */
        return one + (t * t) / (w + w);
    }

    /* |x| in [0.5*ln2,9], return (exp(|x|)+1/exp(|x|))/2; */
    if (ix < 0x41100000) {
        t = __ieee754_expf(fabsf(x));
        return half * t + half / t;
    }

    /* |x| in [9, log(maxfloat)] return half*exp(|x|) */
    if (ix < 0x42b17217) return half * __ieee754_expf(fabsf(x));

    /* |x| in [log(maxfloat), overflowthresold] */
    if (ix <= 0x42b2d4fc)
        return __ldexp_expf(fabsf(x), -1);

    /* |x| > overflowthresold, cosh(x) overflow */
    return huge * huge;
}
