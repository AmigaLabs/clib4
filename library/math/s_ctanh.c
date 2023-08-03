/*
 * $Id: math_s_ctanh.c,v 1.1 2023-07-19 11:33:52 clib2devs Exp $
 */

#ifndef _COMPLEX_HEADERS_H
#include "complex_headers.h"
#endif /* _COMPLEX_HEADERS_H */

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

double complex
ctanh(double complex z) {
    double x, y;
    double t, beta, s, rho, denom;
    uint32_t hx, ix, lx;

    x = creal(z);
    y = cimag(z);

    EXTRACT_WORDS(hx, lx, x);
    ix = hx & 0x7fffffff;

    /*
     * ctanh(NaN + i 0) = NaN + i 0
     *
     * ctanh(NaN + i y) = NaN + i NaN		for y != 0
     *
     * The imaginary part has the sign of x*sin(2*y), but there's no
     * special effort to get this right.
     *
     * ctanh(+-Inf +- i Inf) = +-1 +- 0
     *
     * ctanh(+-Inf + i y) = +-1 + 0 sin(2y)		for y finite
     *
     * The imaginary part of the sign is unspecified.  This special
     * case is only needed to avoid a spurious invalid exception when
     * y is infinite.
     */
    if (ix >= 0x7ff00000) {
        if ((ix & 0xfffff) | lx)    /* x is NaN */
            return (CMPLX(x, (y == 0 ? y : x * y)));
        SET_HIGH_WORD(x, hx - 0x40000000);    /* x = copysign(1, x) */
        return (CMPLX(x, copysign(0, isinf(y) ? y : sin(y) * cos(y))));
    }

    /*
     * ctanh(x + i NAN) = NaN + i NaN
     * ctanh(x +- i Inf) = NaN + i NaN
     */
    if (!isfinite(y))
        return (CMPLX(y - y, y - y));

    /*
     * ctanh(+-huge + i +-y) ~= +-1 +- i 2sin(2y)/exp(2x), using the
     * approximation sinh^2(huge) ~= exp(2*huge) / 4.
     * We use a modified formula to avoid spurious overflow.
     */
    if (ix >= 0x40360000) {    /* x >= 22 */
        double exp_mx = exp(-fabs(x));
        return (CMPLX(copysign(1, x),
                      4 * sin(y) * cos(y) * exp_mx * exp_mx));
    }

    /* Kahan's algorithm */
    t = tan(y);
    beta = 1.0 + t * t;    /* = 1 / cos^2(y) */
    s = sinh(x);
    rho = sqrt(1 + s * s);    /* = cosh(x) */
    denom = 1 + beta * s * s;
    return (CMPLX((beta * rho * s) / denom, t / denom));
}