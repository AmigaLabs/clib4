/*
 * $Id: math_s_exp2f.c,v 1.4 2023-07-19 12:04:23 clib2devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

#define    TBLBITS    4
#define    TBLSIZE    (1 << TBLBITS)

static const float
        huge = 0x1p100f,
        redux = 0x1.8p23f / TBLSIZE,
        P1 = 0x1.62e430p-1f,
        P2 = 0x1.ebfbe0p-3f,
        P3 = 0x1.c6b348p-5f,
        P4 = 0x1.3b2c9cp-7f;

static volatile float twom100 = 0x1p-100f;

static const double exp2ft[TBLSIZE] = {
        0x1.6a09e667f3bcdp-1,
        0x1.7a11473eb0187p-1,
        0x1.8ace5422aa0dbp-1,
        0x1.9c49182a3f090p-1,
        0x1.ae89f995ad3adp-1,
        0x1.c199bdd85529cp-1,
        0x1.d5818dcfba487p-1,
        0x1.ea4afa2a490dap-1,
        0x1.0000000000000p+0,
        0x1.0b5586cf9890fp+0,
        0x1.172b83c7d517bp+0,
        0x1.2387a6e756238p+0,
        0x1.306fe0a31b715p+0,
        0x1.3dea64c123422p+0,
        0x1.4bfdad5362a27p+0,
        0x1.5ab07dd485429p+0,
};

float
exp2f(float x) {
    double tv, twopk, u, z;
    float t;
    uint32_t hx, ix, i0;
    int32_t k;

    /* Filter out exceptional cases. */
    GET_FLOAT_WORD(hx, x);
    ix = hx & 0x7fffffff;        /* high word of |x| */
    if (ix >= 0x43000000) {            /* |x| >= 128 */
        if (ix >= 0x7f800000) {
            if ((ix & 0x7fffff) != 0 || (hx & 0x80000000) == 0)
                return (x + x);    /* x is NaN or +Inf */
            else
                return (0.0);    /* x is -Inf */
        }
        if (x >= 0x1.0p7f)
            return (huge * huge);    /* overflow */
        if (x <= -0x1.2cp7f)
            return (twom100 * twom100); /* underflow */
    } else if (ix <= 0x33000000) {        /* |x| <= 0x1p-25 */
        return (1.0f + x);
    }

    /* Reduce x, computing z, i0, and k. */
    STRICT_ASSIGN(float, t, x + redux);
    GET_FLOAT_WORD(i0, t);
    i0 += TBLSIZE / 2;
    k = (i0 >> TBLBITS) << 20;
    i0 &= TBLSIZE - 1;
    t -= redux;
    z = x - t;
    INSERT_WORDS(twopk, 0x3ff00000 + k, 0);

    /* Compute r = exp2(y) = exp2ft[i0] * p(z). */
    tv = exp2ft[i0];
    u = tv * z;
    tv = tv + u * (P1 + z * P2) + u * (z * z) * (P3 + z * P4);

    /* Scale by 2**(k>>20). */
    return (tv * twopk);
}
