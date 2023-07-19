/*
 * $Id: math_s_ccosh.c,v 1.1 2023-07-19 11:33:52 clib2devs Exp $
 */

#ifndef _COMPLEX_HEADERS_H
#include "complex_headers.h"
#endif /* _COMPLEX_HEADERS_H */

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

static const double huge = 0x1p1023;

double complex
ccosh(double complex z) {
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
            return (CMPLX(cosh(x), x * y));
        if (ix < 0x40360000)    /* small x: normal case */
            return (CMPLX(cosh(x) * cos(y), sinh(x) * sin(y)));

        /* |x| >= 22, so cosh(x) ~= exp(|x|) */
        if (ix < 0x40862e42) {
            /* x < 710: exp(|x|) won't overflow */
            h = exp(fabs(x)) * 0.5;
            return (CMPLX(h * cos(y), copysign(h, x) * sin(y)));
        } else if (ix < 0x4096bbaa) {
            /* x < 1455: scale to avoid overflow */
            z = __ldexp_cexp(CMPLX(fabs(x), y), -1);
            return (CMPLX(creal(z), cimag(z) * copysign(1, x)));
        } else {
            /* x >= 1455: the result always overflows */
            h = huge * x;
            return (CMPLX(h * h * cos(y), h * sin(y)));
        }
    }

    /*
     * cosh(+-0 +- I Inf) = dNaN + I sign(d(+-0, dNaN))0.
     * The sign of 0 in the result is unspecified.  Choice = normally
     * the same as dNaN.  Raise the invalid floating-point exception.
     *
     * cosh(+-0 +- I NaN) = d(NaN) + I sign(d(+-0, NaN))0.
     * The sign of 0 in the result is unspecified.  Choice = normally
     * the same as d(NaN).
     */
    if ((ix | lx) == 0 && iy >= 0x7ff00000)
        return (CMPLX(y - y, copysign(0, x * (y - y))));

    /*
     * cosh(+-Inf +- I 0) = +Inf + I (+-)(+-)0.
     *
     * cosh(NaN +- I 0)   = d(NaN) + I sign(d(NaN, +-0))0.
     * The sign of 0 in the result is unspecified.
     */
    if ((iy | ly) == 0 && ix >= 0x7ff00000) {
        if (((hx & 0xfffff) | lx) == 0)
            return (CMPLX(x * x, copysign(0, x) * y));
        return (CMPLX(x * x, copysign(0, (x + x) * y)));
    }

    /*
     * cosh(x +- I Inf) = dNaN + I dNaN.
     * Raise the invalid floating-point exception for finite nonzero x.
     *
     * cosh(x + I NaN) = d(NaN) + I d(NaN).
     * Optionally raises the invalid floating-point exception for finite
     * nonzero x.  Choice = don't raise (except for signaling NaNs).
     */
    if (ix < 0x7ff00000 && iy >= 0x7ff00000)
        return (CMPLX(y - y, x * (y - y)));

    /*
     * cosh(+-Inf + I NaN)  = +Inf + I d(NaN).
     *
     * cosh(+-Inf +- I Inf) = +Inf + I dNaN.
     * The sign of Inf in the result is unspecified.  Choice = always +.
     * Raise the invalid floating-point exception.
     *
     * cosh(+-Inf + I y)   = +Inf cos(y) +- I Inf sin(y)
     */
    if (ix >= 0x7ff00000 && ((hx & 0xfffff) | lx) == 0) {
        if (iy >= 0x7ff00000)
            return (CMPLX(x * x, x * (y - y)));
        return (CMPLX((x * x) * cos(y), x * sin(y)));
    }

    /*
     * cosh(NaN + I NaN)  = d(NaN) + I d(NaN).
     *
     * cosh(NaN +- I Inf) = d(NaN) + I d(NaN).
     * Optionally raises the invalid floating-point exception.
     * Choice = raise.
     *
     * cosh(NaN + I y)    = d(NaN) + I d(NaN).
     * Optionally raises the invalid floating-point exception for finite
     * nonzero y.  Choice = don't raise (except for signaling NaNs).
     */
    return (CMPLX((x * x) * (y - y), (x + x) * (y - y)));
}
