/*
 * $Id: math_sinf.c,v 1.4 2022-07-15 12:04:24 clib2devs Exp $
 */

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

static const double
        S1 = -0x15555554cbac77.0p-55,    /* -0.166666666416265235595 */
        S2 = 0x111110896efbb2.0p-59,    /*  0.0083333293858894631756 */
        S3 = -0x1a00f9e2cae774.0p-65,    /* -0.000198393348360966317347 */
        S4 = 0x16cd878c3b46a7.0p-71,    /*  0.0000027183114939898219064 */
        pio4 = 7.8539801e-1,
        s1pio2 = 1 * M_PI_2,            /* 0x3FF921FB, 0x54442D18 */
        s2pio2 = 2 * M_PI_2,            /* 0x400921FB, 0x54442D18 */
        s3pio2 = 3 * M_PI_2,            /* 0x4012D97C, 0x7F3321D2 */
        s4pio2 = 4 * M_PI_2;            /* 0x401921FB, 0x54442D18 */

float
__kernel_sindf(double x) {
    double r, s, w, z;

    /* Try to optimize for parallel evaluation as in k_tanf.c. */
    z = x * x;
    w = z * z;
    r = S3 + z * S4;
    s = z * x;
    return (x + s * (S1 + z * S2)) + s * w * r;
}

float
sinf(float x) {
    float y[2], z = 0.0;
    float ix;
    int32_t n;

    ix = fabsf(x);

    /* |x| ~< pi/4 */
    if (ix <= pio4) {
        return __kernel_sinf(x, z, 0);
        /* sin(Inf or NaN) is NaN */
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
                return __kernel_sinf(y[0], y[1], 1);
            case 1:
                return __kernel_cosf(y[0], y[1]);
            case 2:
                return -__kernel_sinf(y[0], y[1], 1);
            default:
                return -__kernel_cosf(y[0], y[1]);
        }
    }
}
