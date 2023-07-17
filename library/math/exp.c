/*
 * $Id: math_exp.c,v 1.8 2021-01-31 09:02:51 clib2devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

static const __float64
        one = _F_64(1.0),
        halF[2] = {
            _F_64(0.5),
            _F_64(-0.5),
        },
        huge = _F_64(1.0e+300),
        twom1000 = _F_64(9.33263618503218878990e-302),      /* 2**-1000=0x01700000,0*/
        o_threshold = _F_64(7.09782712893383973096e+02),    /* 0x40862E42, 0xFEFA39EF */
        u_threshold = _F_64(-7.45133219101941108420e+02),    /* 0xc0874910, 0xD52D3051 */
        ln2HI[2] = {
            _F_64(6.93147180369123816490e-01), /* 0x3fe62e42, 0xfee00000 */
            _F_64(-6.93147180369123816490e-01),
        }, /* 0xbfe62e42, 0xfee00000 */
        ln2LO[2] = {
            _F_64(1.90821492927058770002e-10), /* 0x3dea39ef, 0x35793c76 */
            _F_64(-1.90821492927058770002e-10),
        },                                         /* 0xbdea39ef, 0x35793c76 */
        invln2 = _F_64(1.44269504088896338700e+00), /* 0x3ff71547, 0x652b82fe */
        P1 = _F_64(1.66666666666666019037e-01),     /* 0x3FC55555, 0x5555553E */
        P2 = _F_64(-2.77777777770155933842e-03),     /* 0xBF66C16C, 0x16BEBD93 */
        P3 = _F_64(6.61375632143793436117e-05),     /* 0x3F11566A, 0xAF25DE2C */
        P4 = _F_64(-1.65339022054652515390e-06),     /* 0xBEBBBD41, 0xC5D26BF1 */
        P5 = _F_64(4.13813679705723846039e-08);     /* 0x3E663769, 0x72BEA4D0 */

inline static double
__exp(double x) {
    double y, hi = 0, lo = 0, c, t;
    int k = 0, xsb;
    unsigned int hx;

    GET_HIGH_WORD(hx, x);
    xsb = (hx >> 31) & 1; /* sign bit of x */
    hx &= 0x7fffffff;      /* high word of |x| */

    /* filter out non-finite argument */
    if (hx >= 0x40862E42) { /* if |x|>=709.78... */
        if (hx >= 0x7ff00000) {
            unsigned int lx;

            GET_LOW_WORD(lx, x);
            if (((hx & 0xfffff) | lx) != 0)
                return x + x; /* NaN */
            else
                return (xsb == 0) ? x : (double) 0.0; /* exp(+-inf)={inf,0} */
        }
        if (x > o_threshold)
            return huge * huge; /* overflow */
        if (x < u_threshold)
            return twom1000 * twom1000; /* underflow */
    }

    /* argument reduction */
    if (hx > 0x3fd62e42) { /* if  |x| > 0.5 ln2 */
        if (hx < 0x3FF0A2B2) { /* and |x| < 1.5 ln2 */
            hi = x - ln2HI[xsb];
            lo = ln2LO[xsb];
            k = 1 - xsb - xsb;
        } else {
            k = (int) (invln2 * x + halF[xsb]);
            t = k;
            hi = x - t * ln2HI[0]; /* t*ln2HI is exact here */
            lo = t * ln2LO[0];
        }
        x = hi - lo;
    } else if (hx < 0x3e300000) { /* when |x|<2**-28 */
        if (huge + x > one)
            return one + x; /* trigger inexact */
    } else
        k = 0;

    /* x is now in primary range */
    t = x * x;
    c = x - t * (P1 + t * (P2 + t * (P3 + t * (P4 + t * P5))));
    if (k == 0)
        return one - ((x * c) / (c - (double) 2.0) - x);
    else
        y = one - ((lo - (x * c) / ((double) 2.0 - c)) - hi);

    if (k >= -1021) {
        unsigned int hy;
        GET_HIGH_WORD(hy, y);
        SET_HIGH_WORD(y, hy + (k << 20)); /* add k to y's exponent */
        return y;
    } else {
        unsigned int hy;
        GET_HIGH_WORD(hy, y);
        SET_HIGH_WORD(y, hy + ((k + 1000) << 20)); /* add k to y's exponent */
        return y * twom1000;
    }
}

double
exp(double x) {
    double result;

    result = __exp(x);

    return (result);
}
