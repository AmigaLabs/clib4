/*
 * $Id: math_s_asinhf.c,v 1.5 2023-07-19 12:04:23 clib2devs Exp $
 */

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

static const float
        one = 1.0000000000e+00,     /* 0x3F800000 */
        ln2 = 6.9314718246e-01,     /* 0x3f317218 */
        huge = 1.0000000000e+30;

float asinhf(float x) {
    float t, w;
    int32_t hx, ix;
    GET_FLOAT_WORD(hx, x);
    ix = hx & 0x7fffffff;
    if (ix >= 0x7f800000) return x + x;    /* x is inf or NaN */
    if (ix < 0x31800000) {    /* |x|<2**-28 */
        if (huge + x > one) return x;    /* return x inexact except 0 */
    }
    if (ix > 0x4d800000) {    /* |x| > 2**28 */
        w = __ieee754_logf(fabsf(x)) + ln2;
    } else if (ix > 0x40000000) {    /* 2**28 > |x| > 2.0 */
        t = fabsf(x);
        w = __ieee754_logf((float) 2.0 * t + one / (__ieee754_sqrtf(x * x + one) + t));
    } else {        /* 2.0 > |x| > 2**-28 */
        t = x * x;
        w = log1pf(fabsf(x) + t / (one + __ieee754_sqrtf(one + t)));
    }
    if (hx > 0) return w; else return -w;
}
