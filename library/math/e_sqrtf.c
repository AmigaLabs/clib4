/*
 * $Id: math_e_sqtf.c,v 1.4 2023-07-19 12:04:24 clib2devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

#ifdef __SPE__
/* CN = 1+2**27 = '41a0000002000000' IEEE double format.  Use it to split a
   double for better accuracy.  */
#define  CN   134217729.0
#define  EMULV(x, y, z, zz)          \
    ({  __typeof__ (x) __p, hx, tx, hy, ty;          \
        __p = CN * (x);  hx = ((x) - __p) + __p;  tx = (x) - hx; \
        __p = CN * (y);  hy = ((y) - __p) + __p;  ty = (y) - hy; \
        z = (x) * (y); zz = (((hx * hy - z) + hx * ty) + tx * hy) + tx * ty; \
    })


#include "root.tbl"
#else
static const float one = 1.0, tiny = 1.0e-30;
#endif

float
__ieee754_sqrtf(float x) {
    float z;
#ifndef __SPE__
    asm ("fsqrts %0,%1\n" :"=f" (z):"f" (x));
#else
#if 0
    int32_t sign = (int) 0x80000000;
    int32_t ix, s, q, m, t, i;
    uint32_t r;

    GET_FLOAT_WORD(ix, x);

    /* take care of Inf and NaN */
    if ((ix & 0x7f800000) == 0x7f800000) {
        return x * x + x;        /* sqrt(NaN)=NaN, sqrt(+inf)=+inf
					   sqrt(-inf)=sNaN */
    }
    /* take care of zero */
    if (ix <= 0) {
        if ((ix & (~sign)) == 0)
            return x;/* sqrt(+-0) = +-0 */
        else if (ix < 0)
            return (x - x) / (x - x);        /* sqrt(-ve) = sNaN */
    }
    /* normalize x */
    m = (ix >> 23);
    if (m == 0) {                /* subnormal x */
        for (i = 0; (ix & 0x00800000) == 0; i++) ix <<= 1;
        m -= i - 1;
    }
    m -= 127;    /* unbias exponent */
    ix = (ix & 0x007fffff) | 0x00800000;
    if (m & 1)    /* odd m, double x to make it even */
        ix += ix;
    m >>= 1;    /* m = [m/2] */

    /* generate sqrt(x) bit by bit */
    ix += ix;
    q = s = 0;        /* q = sqrt(x) */
    r = 0x01000000;        /* r = moving bit from right to left */

    while (r != 0) {
        t = s + r;
        if (t <= ix) {
            s = t + r;
            ix -= t;
            q += r;
        }
        ix += ix;
        r >>= 1;
    }

    /* use floating add to find out rounding direction */
    if (ix != 0) {
        z = one - tiny; /* trigger inexact flag */
        if (z >= one) {
            z = one + tiny;
            if (z > one)
                q += 2;
            else
                q += (q & 1);
        }
    }
    ix = (q >> 1) + 0x3f000000;
    ix += (m << 23);
    SET_FLOAT_WORD(z, ix);
#else
    /* Use generic implementation.  */
    static const double
            rt0 = 9.99999999859990725855365213134618E-01,
            rt1 = 4.99999999495955425917856814202739E-01,
            rt2 = 3.75017500867345182581453026130850E-01,
            rt3 = 3.12523626554518656309172508769531E-01;
    static const double big = 134217728.0;
    double y, t, del, res, res1, hy, z1, zz, s;
    mynumber a, c = {{0, 0}};
    int4 k;

    a.x = x;
    k = a.i[HIGH_HALF];
    a.i[HIGH_HALF] = (k & 0x001fffff) | 0x3fe00000;
    t = inroot[(k & 0x001fffff) >> 14];
    s = a.x;
    /*----------------- 2^-1022  <= | x |< 2^1024  -----------------*/
    if (k > 0x000fffff && k < 0x7ff00000) {
        int rm = fegetround();
        fenv_t env;
        feholdexcept(&env);
        fesetround(FE_TONEAREST);
        double ret;
        y = 1.0 - t * (t * s);
        t = t * (rt0 + y * (rt1 + y * (rt2 + y * rt3)));
        c.i[HIGH_HALF] = 0x20000000 + ((k & 0x7fe00000) >> 1);
        y = t * s;
        hy = (y + big) - big;
        del = 0.5 * t * ((s - hy * hy) - (y - hy) * (y + hy));
        res = y + del;
        if (res == (res + 1.002 * ((y - res) + del)))
            ret = res * c.x;
        else {
            res1 = res + 1.5 * ((y - res) + del);
            EMULV(res, res1, z1, zz); /* (z1+zz)=res*res1 */
            res = ((((z1 - s) + zz) < 0) ? max (res, res1) :
                   min (res, res1));
            ret = res * c.x;
        }
        math_force_eval(ret);
        fesetenv(&env);
        double dret = x / ret;
        if (dret != ret) {
            double force_inexact = 1.0 / 3.0;
            math_force_eval(force_inexact);
            /* The square root is inexact, ret is the round-to-nearest
               value which may need adjusting for other rounding
               modes.  */
            switch (rm) {
                case FE_UPWARD:
                  if (dret > ret)
                ret = (res + 0x1p-1022) * c.x;
                  break;
                case FE_DOWNWARD:
                case FE_TOWARDZERO:
                    if (dret < ret)
                        ret = (res - 0x1p-1022) * c.x;
                    break;
                default:
                    break;
            }
        }
        /* Otherwise (x / ret == ret), either the square root was exact or the division was inexact.  */
        return ret;
    } else {
        if ((k & 0x7ff00000) == 0x7ff00000)
            return x * x + x; /* sqrt(NaN)=NaN, sqrt(+inf)=+inf, sqrt(-inf)=sNaN */
        if (x == 0)
            return x;       /* sqrt(+0)=+0, sqrt(-0)=-0 */
        if (k < 0)
            return (x - x) / (x - x); /* sqrt(-ve)=sNaN */
        return 0x1p-256 * __ieee754_sqrt(x * 0x1p512);
    }
#endif
#endif
    return z;
}
