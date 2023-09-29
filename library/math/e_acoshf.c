/*
 * $Id: math_e_acoshf.c,v 1.5 2023-07-19 12:04:23 clib4devs Exp $
 */

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

static const float
        one = 1.0,
        ln2 = 6.9314718246e-01;  /* 0x3f317218 */

float
__ieee754_acoshf(float x) {
    float t;
    int32_t hx;
    GET_FLOAT_WORD(hx, x);
    if (hx < 0x3f800000) {        /* x < 1 */
        return (x - x) / (x - x);
    } else if (hx >= 0x4d800000) {    /* x > 2**28 */
        if (hx >= 0x7f800000) {    /* x is inf of NaN */
            return x + x;
        } else
            return __ieee754_logf(x) + ln2;    /* acosh(huge)=log(2x) */
    } else if (hx == 0x3f800000) {
        return 0.0;            /* acosh(1) = 0 */
    } else if (hx > 0x40000000) {    /* 2**28 > x > 2 */
        t = x * x;
        return __ieee754_logf((float) 2.0 * x - one / (x + __ieee754_sqrtf(t - one)));
    } else {            /* 1<x<2 */
        t = x - one;
        return log1pf(t + __ieee754_sqrtf((float) 2.0 * t + t * t));
    }
}
