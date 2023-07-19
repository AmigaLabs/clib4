/*
 * $Id: math_s_csinh.c,v 1.1 2023-07-19 11:33:52 clib2devs Exp $
 */

#ifndef _COMPLEX_HEADERS_H
#include "complex_headers.h"
#endif /* _COMPLEX_HEADERS_H */

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

static const double huge = 0x1p1023;

double complex
csinh(double complex z) {
    double x, y, h;
    int32_t hx, hy, ix, iy, lx, ly;

    x = creal(z);
    y = cimag(z);

    EXTRACT_WORDS(hx, lx, x);
    EXTRACT_WORDS(hy, ly, y);

    ix = 0x7fffffff & hx;
    iy = 0x7fffffff & hy;

    /* Handle the nearly-non-exceptional cases where x and y are finite. */
    if (ix < 0x7ff00000 && iy < 0x7ff00000) {
        if ((iy | ly) == 0)
            return (CMPLX(sinh(x), y));
        if (ix < 0x40360000)    /* small x: normal case */
            return (CMPLX(sinh(x) * cos(y), cosh(x) * sin(y)));

        /* |x| >= 22, so cosh(x) ~= exp(|x|) */
        if (ix < 0x40862e42) {
            /* x < 710: exp(|x|) won't overflow */
            h = exp(fabs(x)) * 0.5;
            return (CMPLX(copysign(h, x) * cos(y), h * sin(y)));
        } else if (ix < 0x4096bbaa) {
            /* x < 1455: scale to avoid overflow */
            z = __ldexp_cexp(CMPLX(fabs(x), y), -1);
            return (CMPLX(creal(z) * copysign(1, x), cimag(z)));
        } else {
            /* x >= 1455: the result always overflows */
            h = huge * x;
            return (CMPLX(h * cos(y), h * h * sin(y)));
        }
    }

    /*
     * sinh(+-0 +- I Inf) = sign(d(+-0, dNaN))0 + I dNaN.
     * The sign of 0 in the result is unspecified.  Choice = normally
     * the same as dNaN.  Raise the invalid floating-point exception.
     *
     * sinh(+-0 +- I NaN) = sign(d(+-0, NaN))0 + I d(NaN).
     * The sign of 0 in the result is unspecified.  Choice = normally
     * the same as d(NaN).
     */
    if ((ix | lx) == 0 && iy >= 0x7ff00000)
        return (CMPLX(copysign(0, x * (y - y)), y - y));

    /*
     * sinh(+-Inf +- I 0) = +-Inf + I +-0.
     *
     * sinh(NaN +- I 0)   = d(NaN) + I +-0.
     */
    if ((iy | ly) == 0 && ix >= 0x7ff00000) {
        if (((hx & 0xfffff) | lx) == 0)
            return (CMPLX(x, y));
        return (CMPLX(x, copysign(0, y)));
    }

    /*
     * sinh(x +- I Inf) = dNaN + I dNaN.
     * Raise the invalid floating-point exception for finite nonzero x.
     *
     * sinh(x + I NaN) = d(NaN) + I d(NaN).
     * Optionally raises the invalid floating-point exception for finite
     * nonzero x.  Choice = don't raise (except for signaling NaNs).
     */
    if (ix < 0x7ff00000 && iy >= 0x7ff00000)
        return (CMPLX(y - y, x * (y - y)));

    /*
     * sinh(+-Inf + I NaN)  = +-Inf + I d(NaN).
     * The sign of Inf in the result is unspecified.  Choice = normally
     * the same as d(NaN).
     *
     * sinh(+-Inf +- I Inf) = +Inf + I dNaN.
     * The sign of Inf in the result is unspecified.  Choice = always +.
     * Raise the invalid floating-point exception.
     *
     * sinh(+-Inf + I y)   = +-Inf cos(y) + I Inf sin(y)
     */
    if (ix >= 0x7ff00000 && ((hx & 0xfffff) | lx) == 0) {
        if (iy >= 0x7ff00000)
            return (CMPLX(x * x, x * (y - y)));
        return (CMPLX(x * cos(y), INFINITY * sin(y)));
    }

    /*
     * sinh(NaN + I NaN)  = d(NaN) + I d(NaN).
     *
     * sinh(NaN +- I Inf) = d(NaN) + I d(NaN).
     * Optionally raises the invalid floating-point exception.
     * Choice = raise.
     *
     * sinh(NaN + I y)    = d(NaN) + I d(NaN).
     * Optionally raises the invalid floating-point exception for finite
     * nonzero y.  Choice = don't raise (except for signaling NaNs).
     */
    return (CMPLX((x * x) * (y - y), (x + x) * (y - y)));
}
