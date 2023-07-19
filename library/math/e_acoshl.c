/*
 * $Id: math_e_acoshl.c,v 1.1 2023-07-19 12:04:23 clib2devs Exp $
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
    uint64_t lx = 0;
    int64_t hx = 0;
    GET_LDOUBLE_WORDS64(hx, lx, x);
    if (hx < 0x3fff000000000000LL) {        /* x < 1 */
        return (x - x) / (x - x);
    } else if (hx >= 0x4035000000000000LL) {    /* x > 2**54 */
        if (hx >= 0x7fff000000000000LL) {    /* x is inf of NaN */
            return x + x;
        } else
            return logl(x) + ln2;    /* acoshl(huge)=logl(2x) */
    } else if (((hx - 0x3fff000000000000LL) | lx) == 0) {
        return 0.0L;            /* acosh(1) = 0 */
    } else if (hx > 0x4000000000000000LL) {    /* 2**28 > x > 2 */
        t = x * x;
        return logl(2.0L * x - one / (x + sqrtl(t - one)));
    } else {            /* 1<x<2 */
        t = x - one;
        return log1pl(t + sqrtl(2.0L * t + t * t));
    }
}
