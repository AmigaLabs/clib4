/*
 * $Id: math_e_acosl.c,v 1.1 2023-07-19 12:04:23 clib2devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

#include "invtrig.h"

static const long double
        one = 1.00000000000000000000e+00,
        pi = 3.14159265358979323846264338327950280e+00L;

long double
acosl(long double x) {
    union IEEEl2bits u = { 0 };
    long double z, p, q, r, w, s, c, df;
    int16_t expsign, expt;
    u.e = x;
    expsign = u.xbits.expsign;
    expt = expsign & 0x7fff;
    if (expt >= BIAS) {    /* |x| >= 1 */
        if (expt == BIAS && ((u.bits.manh & ~LDBL_NBIT) | u.bits.manl) == 0) {
            if (expsign > 0) return 0.0;    /* acos(1) = 0  */
            else return pi + 2.0 * pio2_lo;    /* acos(-1)= pi */
        }
        return (x - x) / (x - x);        /* acos(|x|>1) is NaN */
    }
    if (expt < BIAS - 1) {    /* |x| < 0.5 */
        if (expt < ACOS_CONST) return pio2_hi + pio2_lo;/*x tiny: acosl=pi/2*/
        z = x * x;
        p = P(z);
        q = Q(z);
        r = p / q;
        return pio2_hi - (x - (pio2_lo - x * r));
    } else if (expsign < 0) {    /* x < -0.5 */
        z = (one + x) * 0.5;
        p = P(z);
        q = Q(z);
        s = sqrtl(z);
        r = p / q;
        w = r * s - pio2_lo;
        return pi - 2.0 * (s + w);
    } else {            /* x > 0.5 */
        z = (one - x) * 0.5;
        s = sqrtl(z);
        u.e = s;
        u.bits.manl = 0;
        df = u.e;
        c = (z - df * df) / (s + df);
        p = P(z);
        q = Q(z);
        r = p / q;
        w = r * s + c;
        return 2.0 * (df + w);
    }
}
