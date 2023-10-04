/*
 * $Id: math_s_csqrtf.c,v 1.1 2023-07-19 11:33:52 clib4devs Exp $
 */

#ifndef _COMPLEX_HEADERS_H
#include "complex_headers.h"
#endif /* _COMPLEX_HEADERS_H */

float complex
csqrtf(float complex z) {
    float a = crealf(z), b = cimagf(z);
    double t;

    /* Handle special cases. */
    if (z == 0)
        return (CMPLXF(0, b));
    if (isinf(b))
        return (CMPLXF(INFINITY, b));
    if (isnan(a)) {
        t = (b - b) / (b - b);    /* raise invalid if b is not a NaN */
        return (CMPLXF(a, t));    /* return NaN + NaN i */
    }
    if (isinf(a)) {
        /*
         * csqrtf(inf + NaN i)  = inf +  NaN i
         * csqrtf(inf + y i)    = inf +  0 i
         * csqrtf(-inf + NaN i) = NaN +- inf i
         * csqrtf(-inf + y i)   = 0   +  inf i
         */
        if (signbit(a))
            return (CMPLXF(fabsf(b - b), copysignf(a, b)));
        else
            return (CMPLXF(a, copysignf(b - b, b)));
    }
    /*
     * The remaining special case (b is NaN) is handled just fine by
     * the normal code path below.
     */

    /*
     * We compute t in double precision to avoid overflow and to
     * provide correct rounding in nearly all cases.
     * This is Algorithm 312, CACM vol 10, Oct 1967.
     */
    if (a >= 0) {
        t = sqrt((a + hypot(a, b)) * 0.5);
        return (CMPLXF(t, b / (2.0 * t)));
    } else {
        t = sqrt((-a + hypot(a, b)) * 0.5);
        return (CMPLXF(fabsf(b) / (2.0 * t), copysignf(t, b)));
    }
}
