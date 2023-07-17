/*
 * $Id: math_sqrt.c,v 1.12 2021-01-31 07:54:24 clib2devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

double
sqrt(double x) {
    __float64 z;
    uint32_t sign = 0x80000000;
    uint32_t r, t1, s1, ix1, q1;
    int32_t ix0, s0, q, m, t, i;

    EXTRACT_WORDS(ix0, ix1, x);

    /* take care of Inf and NaN */
    if ((ix0 & 0x7ff00000) == 0x7ff00000) {
        if (ix0 < 0 && !isnan(x))
            return __math_invalid(x); /* sqrt(-inf)=sNaN */
        return x + x; /* sqrt(NaN)=NaN, sqrt(+inf)=+inf */
    }
    /* take care of zero */
    if (ix0 <= 0) {
        if (((ix0 & (~sign)) | ix1) == 0)
            return x; /* sqrt(+-0) = +-0 */
        else if (ix0 < 0)
            return __math_invalid(x); /* sqrt(-ve) = sNaN */
    }
    /* normalize x */
    m = (ix0 >> 20);
    if (m == 0) { /* subnormal x */
        while (ix0 == 0) {
            m -= 21;
            ix0 |= (ix1 >> 11);
            ix1 <<= 21;
        }
        for (i = 0; (ix0 & 0x00100000) == 0; i++)
            ix0 <<= 1;
        m -= i - 1;
        if (i)
            ix0 |= (ix1 >> (32 - i));
        ix1 <<= i;
    }
    m -= 1023; /* unbias exponent */
    ix0 = (ix0 & 0x000fffff) | 0x00100000;
    if (m & 1) { /* odd m, double x to make it even */
        ix0 += ix0 + ((ix1 & sign) >> 31);
        ix1 += ix1;
    }
    m >>= 1; /* m = [m/2] */

    /* generate sqrt(x) bit by bit */
    ix0 += ix0 + ((ix1 & sign) >> 31);
    ix1 += ix1;
    q = q1 = s0 = s1 = 0; /* [q,q1] = sqrt(x) */
    r = 0x00200000; /* r = moving bit from right to left */

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
            if (((t1 & sign) == sign) && (s1 & sign) == 0)
                s0 += 1;
            ix0 -= t;
            if (ix1 < t1)
                ix0 -= 1;
            ix1 -= t1;
            q1 += r;
        }
        ix0 += ix0 + ((ix1 & sign) >> 31);
        ix1 += ix1;
        r >>= 1;
    }

    if ((ix0 | ix1) != 0) {
        FE_DECL_ROUND(rnd);
        if (__is_nearest(rnd) || __is_upward(rnd)) {
            if (q1 == (uint32_t) 0xffffffff) {
                q1 = 0;
                q += 1;
            } else if (__is_upward(rnd)) {
                if (q1 == (uint32_t) 0xfffffffe)
                    q += 1;
                q1 += 2;
            } else
                q1 += (q1 & 1);
        }
    }
    ix0 = (q >> 1) + 0x3fe00000;
    ix1 = q1 >> 1;
    if ((q & 1) == 1)
        ix1 |= sign;
    ix0 += (m << 20);
    INSERT_WORDS(z, ix0, ix1);
    return z;
}
