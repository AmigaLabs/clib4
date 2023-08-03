/*
 * $Id: math_e_sinh.c,v 1.6 2023-07-19 12:04:24 clib2devs Exp $
*/

#ifndef _COMPLEX_HEADERS_H
#include "complex_headers.h"
#endif /* _COMPLEX_HEADERS_H */

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

static const double one = 1.0, shuge = 1.0e307;

double
__ieee754_sinh(double x) {
    double t, h;
    int32_t ix, jx;

    /* High word of |x|. */
    GET_HIGH_WORD(jx, x);
    ix = jx & 0x7fffffff;

    /* x is INF or NaN */
    if (ix >= 0x7ff00000) return x + x;

    h = 0.5;
    if (jx < 0) h = -h;
    /* |x| in [0,22], return sign(x)*0.5*(E+E/(E+1))) */
    if (ix < 0x40360000) {        /* |x|<22 */
        if (ix < 0x3e300000)        /* |x|<2**-28 */
            if (shuge + x > one) return x;/* sinh(tiny) = tiny with inexact */
        t = expm1(fabs(x));
        if (ix < 0x3ff00000) return h * (2.0 * t - t * t / (t + one));
        return h * (t + t / (t + one));
    }

    /* |x| in [22, log(maxdouble)] return 0.5*exp(|x|) */
    if (ix < 0x40862E42) return h * __ieee754_exp(fabs(x));

    /* |x| in [log(maxdouble), overflowthresold] */
    if (ix <= 0x408633CE)
        return h * 2.0 * __ldexp_exp(fabs(x), -1);

    /* |x| > overflowthresold, sinh(x) overflow */
    return x * shuge;
}
