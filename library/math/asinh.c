/*
 * $Id: math_asinh.c,v 1.4 2023-07-15 12:04:23 clib2devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

static const __float64
        one  = _F_64(1.00000000000000000000e+00), /* 0x3FF00000, 0x00000000 */
        ln2  = _F_64(6.93147180559945286227e-01), /* 0x3FE62E42, 0xFEFA39EF */
        huge = _F_64(1.00000000000000000000e+300);

double
asinh(double x) {
    double w;
    int32_t hx, ix;
    GET_HIGH_WORD(hx, x);
    ix = hx & 0x7fffffff;
    if (ix < 0x3e300000) {                     /* |x|<2**-28 */
        if (huge + x > one) return x;          /* return x inexact except 0 */
    }
    if (ix > 0x41b00000) {                     /* |x| > 2**28 */
        if (ix >= 0x7ff00000) return x + x;    /* x is inf or NaN */
        w = log(fabs(x)) + ln2;
    } else {
        double xa = fabs(x);
        if (ix > 0x40000000) {                 /* 2**28 > |x| > 2.0 */
            w = log((double) 2.0 * xa + one / (sqrt(xa * xa + one) + xa));
        } else {                               /* 2.0 > |x| > 2**-28 */
            double t = xa * xa;
            w = log1p(xa + t / (one + sqrt(one + t)));
        }
    }
    return copysign(w, x);
}
