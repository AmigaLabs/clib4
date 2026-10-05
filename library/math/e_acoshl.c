/*
 * $Id: math_e_acoshl.c,v 1.1 2023-07-19 12:04:23 clib4devs Exp $
 */

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

static const long double
        one = 1.0,
        ln2 = 0.6931471805599453094172321214581766L;

long double
acoshl(long double x) {
    long double t;
    if (x < one) {        /* x < 1 */
        return (x - x) / (x - x);
    } else if (x >= 0x1p54L) {    /* x > 2**54 */
        if (!isfinite(x)) {    /* x is inf of NaN */
            return x + x;
        } else
            return logl(x) + ln2;    /* acoshl(huge)=logl(2x) */
    } else if (x == one) {
        return 0.0L;            /* acosh(1) = 0 */
    } else if (x > 2.0L) {    /* 2**28 > x > 2 */
        t = x * x;
        return logl(2.0L * x - one / (x + sqrtl(t - one)));
    } else {            /* 1<x<2 */
        t = x - one;
        return log1pl(t + sqrtl(2.0L * t + t * t));
    }
}
