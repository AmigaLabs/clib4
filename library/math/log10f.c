/*
 * $Id: math_log10f.c,v 1.4 2022-03-12 12:04:23 clib2devs Exp $
 *

 *
 *
 * PowerPC math library based in part on work by Sun Microsystems
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunPro, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice
 * is preserved.
 *
 */

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

static const float
    two25 = 3.3554432000e+07, /* 0x4c000000 */
    ivln10hi = 4.3432617188e-01, /* 0x3ede6000 */
    ivln10lo = -3.1689971365e-05, /* 0xb804ead9 */
    log10_2hi = 3.0102920532e-01, /* 0x3e9a2080 */
    log10_2lo = 7.9034151668e-07; /* 0x355427db */

static const float zero = 0.0;

static const float
    /* |(log(1+s)-log(1-s))/s - Lg(s)| < 2**-34.24 (~[-4.95e-11, 4.97e-11]). */
    Lg1 = 0xaaaaaa.0p-24,    /* 0.66666662693 */
    Lg2 = 0xccce13.0p-25,    /* 0.40000972152 */
    Lg3 = 0x91e9ee.0p-25,    /* 0.28498786688 */
    Lg4 = 0xf89e26.0p-26;    /* 0.24279078841 */

static inline float
k_log1pf(float f) {
    float hfsq, s, z, R, w, t1, t2;

    s = f / ((float) 2.0 + f);
    z = s * s;
    w = z * z;
    t1 = w * (Lg2 + w * Lg4);
    t2 = z * (Lg1 + w * Lg3);
    R = t2 + t1;
    hfsq = (float) 0.5 * f * f;
    return s * (hfsq + R);
}

float log10f(float x) {
    float f, hfsq, hi, lo, r, y;
    int32_t i, k, hx;

    GET_FLOAT_WORD(hx, x);

    k = 0;
    if (hx < 0x00800000) {            /* x < 2**-126  */
        if ((hx & 0x7fffffff) == 0)
            return -two25 / zero;        /* log(+-0)=-inf */
        if (hx < 0) return (x - x) / zero;    /* log(-#) = NaN */
        k -= 25;
        x *= two25; /* subnormal number, scale up x */
        GET_FLOAT_WORD(hx, x);
    }
    if (hx >= 0x7f800000) return x + x;
    if (hx == 0x3f800000)
        return zero;            /* log(1) = +0 */
    k += (hx >> 23) - 127;
    hx &= 0x007fffff;
    i = (hx + (0x4afb0d)) & 0x800000;
    SET_FLOAT_WORD(x, hx | (i ^ 0x3f800000));    /* normalize x or x/2 */
    k += (i >> 23);
    y = (float) k;
    f = x - (float) 1.0;
    hfsq = (float) 0.5 * f * f;
    r = k_log1pf(f);

    /* See e_log2f.c and e_log2.c for details. */
    if (sizeof(float_t) > sizeof(float))
        return (r - hfsq + f) * ((float_t) ivln10lo + ivln10hi) +
               y * ((float_t) log10_2lo + log10_2hi);
    hi = f - hfsq;
    GET_FLOAT_WORD(hx, hi);
    SET_FLOAT_WORD(hi, hx & 0xfffff000);
    lo = (f - hi) - hfsq + r;
    return y * log10_2lo + (lo + hi) * ivln10lo + lo * ivln10hi + hi * ivln10hi +
           y * log10_2hi;
}
