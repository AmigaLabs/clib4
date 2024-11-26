/*
 * $Id: math_s_log1p.c,v 1.4 2023-07-19 12:04:23 clib4devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

static const double
        ln2_hi = 6.93147180369123816490e-01,    /* 3fe62e42 fee00000 */
        ln2_lo = 1.90821492927058770002e-10,    /* 3dea39ef 35793c76 */
        two54 = 1.80143985094819840000e+16,  /* 43500000 00000000 */
        Lp1 = 6.666666666666735130e-01,  /* 3FE55555 55555593 */
        Lp2 = 3.999999999940941908e-01,  /* 3FD99999 9997FA04 */
        Lp3 = 2.857142874366239149e-01,  /* 3FD24924 94229359 */
        Lp4 = 2.222219843214978396e-01,  /* 3FCC71C5 1D8E78AF */
        Lp5 = 1.818357216161805012e-01,  /* 3FC74664 96CB03DE */
        Lp6 = 1.531383769920937332e-01,  /* 3FC39A09 D078C69F */
        Lp7 = 1.479819860511658591e-01;  /* 3FC2F112 DF3E5244 */

static const double zero = 0.0;

double
log1p(double x) {
    double hfsq, f, c = 0, s, z, R, u;
    int32_t k, hx, hu, ax;

    GET_HIGH_WORD(hx, x);
    ax = hx & 0x7fffffff;

    k = 1;
    if (hx < 0x3FDA827A) {            /* 1+x < sqrt(2)+ */
        if (ax >= 0x3ff00000) {        /* x <= -1.0 */
            if (x == -1.0) return -two54 / zero; /* log1p(-1)=+inf */
            else return (x - x) / (x - x);    /* log1p(x<-1)=NaN */
        }
        if (ax < 0x3e200000) {            /* |x| < 2**-29 */
            if (two54 + x > zero            /* raise inexact */
                && ax < 0x3c900000)        /* |x| < 2**-54 */
                return x;
            else
                return x - x * x * 0.5;
        }
        if (hx > 0 || hx <= ((int32_t) 0xbfd2bec4)) {
            k = 0;
            f = x;
            hu = 1;
        }        /* sqrt(2)/2- <= 1+x < sqrt(2)+ */
    }
    if (hx >= 0x7ff00000) return x + x;
    if (k != 0) {
        if (hx < 0x43400000) {
            STRICT_ASSIGN(double, u, 1.0 + x);
            GET_HIGH_WORD(hu, u);
            k = (hu >> 20) - 1023;
            c = (k > 0) ? 1.0 - (u - x) : x - (u - 1.0);/* correction term */
            c /= u;
        } else {
            u = x;
            GET_HIGH_WORD(hu, u);
            k = (hu >> 20) - 1023;
            c = 0;
        }
        hu &= 0x000fffff;
        /*
         * The approximation to sqrt(2) used in thresholds is not
         * critical.  However, the ones used above must give less
         * strict bounds than the one here so that the k==0 case is
         * never reached from here, since here we have committed to
         * using the correction term but don't use it if k==0.
         */
        if (hu < 0x6a09e) {            /* u ~< sqrt(2) */
            SET_HIGH_WORD(u, hu | 0x3ff00000);    /* normalize u */
        } else {
            k += 1;
            SET_HIGH_WORD(u, hu | 0x3fe00000);    /* normalize u/2 */
            hu = (0x00100000 - hu) >> 2;
        }
        f = u - 1.0;
    }
    hfsq = 0.5 * f * f;
    if (hu == 0) {    /* |f| < 2**-20 */
        if (f == zero) {
            if (k == 0) {
                return zero;
            } else {
                c += k * ln2_lo;
                return k * ln2_hi + c;
            }
        }
        R = hfsq * (1.0 - 0.66666666666666666 * f);
        if (k == 0) return f - R;
        else
            return k * ln2_hi - ((R - (k * ln2_lo + c)) - f);
    }
    s = f / (2.0 + f);
    z = s * s;
    R = z * (Lp1 + z * (Lp2 + z * (Lp3 + z * (Lp4 + z * (Lp5 + z * (Lp6 + z * Lp7))))));
    if (k == 0) return f - (hfsq - s * (hfsq + R));
    else
        return k * ln2_hi - ((hfsq - (s * (hfsq + R) + (k * ln2_lo + c))) - f);
}
