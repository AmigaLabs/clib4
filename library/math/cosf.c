/*
 * $Id: math_cosf.c,v 1.4 2023-07-15 12:04:23 clib2devs Exp $
 */

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

static const double
        one = 1.0,
        C0 = -0x1ffffffd0c5e81.0p-54,    /* -0.499999997251031003120 */
        C1 = 0x155553e1053a42.0p-57,    /*  0.0416666233237390631894 */
        C2 = -0x16c087e80f1e27.0p-62,    /* -0.00138867637746099294692 */
        C3 = 0x199342e0ee5069.0p-68,    /*  0.0000243904487962774090654 */
        pio4 = 7.8539801e-1;

float
__kernel_cosdf(double x) {
    double r, w, z;

    /* Try to optimize for parallel evaluation as in k_tanf.c. */
    z = x * x;
    w = z * z;
    r = C2 + z * C3;
    return ((one + z * C0) + w * C1) + (w * z) * r;
}

float
cosf(float x) {
    float y[2], z = 0.0;
    float ix;
    int32_t n;

    ix = fabsf(x);

    /* |x| ~< pi/4 */
    if ((double) ix <= pio4) {
        return __kernel_cosf(x, z);
        /* cos(Inf or NaN) is NaN */
    } else if (__isnanf(ix)) {
        return x - x;
    } else if (__isinff(ix)) {
        __set_errno(EDOM);
        return x - x;
    }

        /* argument reduction needed */
    else {
        n = __rem_pio2f(x, y);
        switch (n & 3) {
            case 0:
                return __kernel_cosf(y[0], y[1]);
            case 1:
                return -__kernel_sinf(y[0], y[1], 1);
            case 2:
                return -__kernel_cosf(y[0], y[1]);
            default:
                return __kernel_sinf(y[0], y[1], 1);
        }
    }
}
