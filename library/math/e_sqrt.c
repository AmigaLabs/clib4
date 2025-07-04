/*
 * $Id: math_e_sqrt.c,v 1.12 2023-07-18 07:54:24 clib4devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

static	const float	one	= 1.0, tiny = 1.0e-30;

double
__ieee754_sqrt(double x) {
    double z;

#ifndef __SPE__
    int32_t sign = (int32_t) 0x80000000;
    int32_t ix0, s0, q, m, t, i;
    uint32_t r, t1, s1, ix1, q1;

    EXTRACT_WORDS(ix0, ix1, x);

    /* take care of Inf and NaN */
    if ((ix0 & 0x7ff00000) == 0x7ff00000) {
        return x * x + x;        /* sqrt(NaN)=NaN, sqrt(+inf)=+inf sqrt(-inf)=sNaN */
    }
    /* take care of zero */
    if (ix0 <= 0) {
        if (((ix0 & (~sign)) | ix1) == 0) {
            return x;/* sqrt(+-0) = +-0 */
        }
        else if (ix0 < 0) {
            return (x - x) / (x - x);        /* sqrt(-ve) = sNaN */
        }
    }
    /* normalize x */
    m = (ix0 >> 20);
    if (m == 0) {                /* subnormal x */
        while (ix0 == 0) {
            m -= 21;
            ix0 |= (ix1 >> 11);
            ix1 <<= 21;
        }
        for (i = 0; (ix0 & 0x00100000) == 0; i++) ix0 <<= 1;
        m -= i - 1;
        ix0 |= (ix1 >> (32 - i));
        ix1 <<= i;
    }
    m -= 1023;    /* unbias exponent */
    ix0 = (ix0 & 0x000fffff) | 0x00100000;
    if (m & 1) {    /* odd m, double x to make it even */
        ix0 += ix0 + ((ix1 & sign) >> 31);
        ix1 += ix1;
    }
    m >>= 1;    /* m = [m/2] */

    /* generate sqrt(x) bit by bit */
    ix0 += ix0 + ((ix1 & sign) >> 31);
    ix1 += ix1;
    q = q1 = s0 = s1 = 0;    /* [q,q1] = sqrt(x) */
    r = 0x00200000;        /* r = moving bit from right to left */

    while (r != 0) {
        t = s0 + r;
        if (t <= ix0) {
            s0 = t + r;
            ix0 -= t;
            q += r;
        }
        ix0 += ix0 + ((ix1 & sign) >> 31);
        ix1 += ix1;
        r >>= 1;
    }

    r = sign;
    while (r != 0) {
        t1 = s1 + r;
        t = s0;
        if ((t < ix0) || ((t == ix0) && (t1 <= ix1))) {
            s1 = t1 + r;
            if (((int32_t) (t1 & sign) == sign) && (s1 & sign) == 0) s0 += 1;
            ix0 -= t;
            if (ix1 < t1) ix0 -= 1;
            ix1 -= t1;
            q1 += r;
        }
        ix0 += ix0 + ((ix1 & sign) >> 31);
        ix1 += ix1;
        r >>= 1;
    }

    /* use floating add to find out rounding direction */
    if ((ix0 | ix1) != 0) {
        z = one - tiny; /* trigger inexact flag */
        if (z >= one) {
            z = one + tiny;
            if (q1 == (uint32_t) 0xffffffff) {
                q1 = 0;
                q += 1;
            }
            else if (z > one) {
                if (q1 == (uint32_t) 0xfffffffe) q += 1;
                q1 += 2;
            } else
                q1 += (q1 & 1);
        }
    }
    ix0 = (q >> 1) + 0x3fe00000;
    ix1 = q1 >> 1;
    if ((q & 1) == 1) ix1 |= sign;
    ix0 += (m << 20);
    INSERT_WORDS(z, ix0, ix1);
#endif
    return z;
}
