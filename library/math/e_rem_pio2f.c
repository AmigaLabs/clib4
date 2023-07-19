/*
 * $Id: math_e_rem_pio2f.c,v 1.4 2023-07-15 12:18:24 clib2devs Exp $
 */

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

static const double
        invpio2 = 6.36619772367581382433e-01, /* 0x3FE45F30, 0x6DC9C883 */
        pio2_1  = 1.57079631090164184570e+00, /* 0x3FF921FB, 0x50000000 */
        pio2_1t = 1.58932547735281966916e-08; /* 0x3E5110b4, 0x611A6263 */

int
__ieee754_rem_pio2f(float x, double *y) {
    double w, r, fn;
    double tx[1], ty[1];
    float z;
    int32_t e0, n, ix, hx;

    GET_FLOAT_WORD(hx, x);
    ix = hx & 0x7fffffff;
    /* 33+53 bit pi is good enough for medium size */
    if (ix < 0x4dc90fdb) {        /* |x| ~< 2^28*(pi/2), medium size */
        /* Use a specialized rint() to get fn.  Assume round-to-nearest. */
        STRICT_ASSIGN(double, fn, x * invpio2 + 0x1.8p52);
        fn = fn - 0x1.8p52;
#ifdef HAVE_EFFICIENT_IRINT
        n  = irint(fn);
#else
        n = (int32_t) fn;
#endif
        r = x - fn * pio2_1;
        w = fn * pio2_1t;
        *y = r - w;
        return n;
    }
    /*
     * all other (large) arguments
     */
    if (ix >= 0x7f800000) {        /* x is inf or NaN */
        *y = x - x;
        return 0;
    }
    /* set z = scalbn(|x|,ilogb(|x|)-23) */
    e0 = (ix >> 23) - 150;        /* e0 = ilogb(|x|)-23; */
    SET_FLOAT_WORD(z, ix - ((int32_t)(e0 << 23)));
    tx[0] = z;
    n = __kernel_rem_pio2(tx, ty, e0, 1, 0);
    if (hx < 0) {
        *y = -ty[0];
        return -n;
    }
    *y = ty[0];
    return n;
}