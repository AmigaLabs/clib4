/*
 * $Id: complex_csqrt.c,v 1.0 2021-01-30 11:33:52 clib2devs Exp $
 *

 *
 *****************************************************************************
 *
 * Documentation and source code for this library, and the most recent library
 * build are available from <https://github.com/afxgroup/clib2>.
 *
 *****************************************************************************
 * Imported and modified for inclusion in clib2 2021/01/25
 * Ola Soder <rolfkopman@gmail.com>
 */

#ifndef _COMPLEX_HEADERS_H
#include "complex_headers.h"
#endif /* _COMPLEX_HEADERS_H */

/* We risk spurious overflow for components >= DBL_MAX / (1 + sqrt(2)). */
#define    THRESH    0x1.a827999fcef32p+1022

double complex

csqrt(double complex z) {
    double complex
    result;
    double a, b;
    double t;
    int scale;

    a = creal(z);
    b = cimag(z);

    /* Handle special cases. */
    if (z == 0)
        return (CMPLX(0, b));
    if (isinf(b))
        return (CMPLX(INFINITY, b));
    if (isnan(a)) {
        t = (b - b) / (b - b);    /* raise invalid if b is not a NaN */
        return (CMPLX(a, t));    /* return NaN + NaN i */
    }
    if (isinf(a)) {
        /*
         * csqrt(inf + NaN i)  = inf +  NaN i
         * csqrt(inf + y i)    = inf +  0 i
         * csqrt(-inf + NaN i) = NaN +- inf i
         * csqrt(-inf + y i)   = 0   +  inf i
         */
        if (signbit(a))
            return (CMPLX(fabs(b - b), copysign(a, b)));
        else
            return (CMPLX(a, copysign(b - b, b)));
    }
    /*
     * The remaining special case (b is NaN) is handled just fine by
     * the normal code path below.
     */

    /* Scale to avoid overflow. */
    if (fabs(a) >= THRESH || fabs(b) >= THRESH) {
        a *= 0.25;
        b *= 0.25;
        scale = 1;
    } else {
        scale = 0;
    }

    /* Algorithm 312, CACM vol 10, Oct 1967. */
    if (a >= 0) {
        t = sqrt((a + hypot(a, b)) * 0.5);
        result = CMPLX(t, b / (2 * t));
    } else {
        t = sqrt((-a + hypot(a, b)) * 0.5);
        result = CMPLX(fabs(b) / (2 * t), copysign(t, b));
    }

    /* Rescale. */
    if (scale)
        return (result * 2);
    else
        return (result);
}
