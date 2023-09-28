/*
 * $Id: math_s_cbrtf.c,v 1.4 2022-03-13 12:04:23 clib4devs Exp $
 */

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

static const unsigned
        B1 = 709958130, /* B1 = (127-127.0/3-0.03306235651)*2**23 */
        B2 = 642849266; /* B2 = (127-127.0/3-24/3-0.03306235651)*2**23 */

float
cbrtf(float x) {
    double r, T;
    float t;
    int32_t hx;
    uint32_t sign;
    uint32_t high;

    GET_FLOAT_WORD(hx, x);
    sign = hx & 0x80000000;        /* sign= sign(x) */
    hx ^= sign;
    if (hx >= 0x7f800000) return (x + x); /* cbrt(NaN,INF) is itself */

    /* rough cbrt to 5 bits */
    if (hx < 0x00800000) {        /* zero or subnormal? */
        if (hx == 0)
            return (x);        /* cbrt(+-0) is itself */
        SET_FLOAT_WORD(t, 0x4b800000); /* set t= 2**24 */
        t *= x;
        GET_FLOAT_WORD(high, t);
        SET_FLOAT_WORD(t, sign | ((high & 0x7fffffff) / 3 + B2));
    } else
        SET_FLOAT_WORD(t, sign | (hx / 3 + B1));

    /*
     * First step Newton iteration (solving t*t-x/t == 0) to 16 bits.  In
     * double precision so that its terms can be arranged for efficiency
     * without causing overflow or underflow.
     */
    T = t;
    r = T * T * T;
    T = T * ((double) x + (double) x + r) / ((double) x + r + r);

    /*
     * Second step Newton iteration to 47 bits.  In double precision for
     * efficiency and accuracy.
     */
    r = T * T * T;
    T = T * ((double) x + (double) x + r) / ((double) x + r + r);

    /* rounding to 24 bits is perfect in round-to-nearest mode */
    return (T);
}
