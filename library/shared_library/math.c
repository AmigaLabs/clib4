#include <limits.h>
#include <math.h>

#include "math.h"

di_int
__fixsfdi(float a) {
    float_bits fb;
    fb.f = a;
    int e = ((fb.u & 0x7F800000) >> 23) - 127;
    if (e < 0)
        return 0;
    di_int s = (si_int) (fb.u & 0x80000000) >> 31;
    di_int r = (fb.u & 0x007FFFFF) | 0x00800000;
    if (e > 23)
        r <<= (e - 23);
    else
        r >>= (23 - e);
    return (r ^ s) - s;
}

di_int
__fixdfdi(double a) {
    double_bits fb;
    fb.f = a;
    int e = ((fb.u.s.high & 0x7FF00000) >> 20) - 1023;
    if (e < 0)
        return 0;
    di_int s = (si_int) (fb.u.s.high & 0x80000000) >> 31;
    dwords r;
    r.s.high = (fb.u.s.high & 0x000FFFFF) | 0x00100000;
    r.s.low = fb.u.s.low;
    if (e > 52)
        r.all <<= (e - 52);
    else
        r.all >>= (52 - e);
    return (r.all ^ s) - s;
}

du_int
__udivmoddi4(du_int a, du_int b, du_int *rem) {
    const unsigned n_uword_bits = sizeof(su_int) * CHAR_BIT;
    const unsigned n_udword_bits = sizeof(du_int) * CHAR_BIT;
    udwords n;
    n.all = a;
    udwords d;
    d.all = b;
    udwords q;
    udwords r;
    unsigned sr;
    /* special cases, X is unknown, K != 0 */
    if (n.s.high == 0) {
        if (d.s.high == 0) {
            /* 0 X
             * ---
             * 0 X
             */
            if (rem)
                *rem = n.s.low % d.s.low;
            return n.s.low / d.s.low;
        }
        /* 0 X
         * ---
         * K X
         */
        if (rem)
            *rem = n.s.low;
        return 0;
    }
    /* n.s.high != 0 */
    if (d.s.low == 0) {
        if (d.s.high == 0) {
            /* K X
             * ---
             * 0 0
             */
            if (rem)
                *rem = n.s.high % d.s.low;
            return n.s.high / d.s.low;
        }
        /* d.s.high != 0 */
        if (n.s.low == 0) {
            /* K 0
             * ---
             * K 0
             */
            if (rem) {
                r.s.high = n.s.high % d.s.high;
                r.s.low = 0;
                *rem = r.all;
            }
            return n.s.high / d.s.high;
        }
        /* K K
         * ---
         * K 0
         */
        if ((d.s.high & (d.s.high - 1)) == 0)     /* if d is a power of 2 */
        {
            if (rem) {
                r.s.low = n.s.low;
                r.s.high = n.s.high & (d.s.high - 1);
                *rem = r.all;
            }
            return n.s.high >> __builtin_ctz(d.s.high);
        }
        /* K K
         * ---
         * K 0
         */
        sr = __builtin_clz(d.s.high) - __builtin_clz(n.s.high);
        /* 0 <= sr <= n_uword_bits - 2 or sr large */
        if (sr > n_uword_bits - 2) {
            if (rem)
                *rem = n.all;
            return 0;
        }
        ++sr;
        /* 1 <= sr <= n_uword_bits - 1 */
        /* q.all = n.all << (n_udword_bits - sr); */
        q.s.low = 0;
        q.s.high = n.s.low << (n_uword_bits - sr);
        /* r.all = n.all >> sr; */
        r.s.high = n.s.high >> sr;
        r.s.low = (n.s.high << (n_uword_bits - sr)) | (n.s.low >> sr);
    } else  /* d.s.low != 0 */
    {
        if (d.s.high == 0) {
            /* K X
             * ---
             * 0 K
             */
            if ((d.s.low & (d.s.low - 1)) == 0)     /* if d is a power of 2 */
            {
                if (rem)
                    *rem = n.s.low & (d.s.low - 1);
                if (d.s.low == 1)
                    return n.all;
                sr = __builtin_ctz(d.s.low);
                q.s.high = n.s.high >> sr;
                q.s.low = (n.s.high << (n_uword_bits - sr)) | (n.s.low >> sr);
                return q.all;
            }
            /* K X
             * ---
             *0 K
             */
            sr = 1 + n_uword_bits + __builtin_clz(d.s.low) - __builtin_clz(n.s.high);
            /* 2 <= sr <= n_udword_bits - 1
             * q.all = n.all << (n_udword_bits - sr);
             * r.all = n.all >> sr;
             * if (sr == n_uword_bits)
             * {
             *     q.s.low = 0;
             *     q.s.high = n.s.low;
             *     r.s.high = 0;
             *     r.s.low = n.s.high;
             * }
             * else if (sr < n_uword_bits)  // 2 <= sr <= n_uword_bits - 1
             * {
             *     q.s.low = 0;
             *     q.s.high = n.s.low << (n_uword_bits - sr);
             *     r.s.high = n.s.high >> sr;
             *     r.s.low = (n.s.high << (n_uword_bits - sr)) | (n.s.low >> sr);
             * }
             * else              // n_uword_bits + 1 <= sr <= n_udword_bits - 1
             * {
             *     q.s.low = n.s.low << (n_udword_bits - sr);
             *     q.s.high = (n.s.high << (n_udword_bits - sr)) |
             *              (n.s.low >> (sr - n_uword_bits));
             *     r.s.high = 0;
             *     r.s.low = n.s.high >> (sr - n_uword_bits);
             * }
             */
            q.s.low = (n.s.low << (n_udword_bits - sr)) &
                      ((si_int) (n_uword_bits - sr) >> (n_uword_bits - 1));
            q.s.high = ((n.s.low << (n_uword_bits - sr)) &
                        ((si_int) (sr - n_uword_bits - 1) >> (n_uword_bits - 1))) |
                       (((n.s.high << (n_udword_bits - sr)) |
                         (n.s.low >> (sr - n_uword_bits))) &
                        ((si_int) (n_uword_bits - sr) >> (n_uword_bits - 1)));
            r.s.high = (n.s.high >> sr) &
                       ((si_int) (sr - n_uword_bits) >> (n_uword_bits - 1));
            r.s.low = ((n.s.high >> (sr - n_uword_bits)) &
                       ((si_int) (n_uword_bits - sr - 1) >> (n_uword_bits - 1))) |
                      (((n.s.high << (n_uword_bits - sr)) |
                        (n.s.low >> sr)) &
                       ((si_int) (sr - n_uword_bits) >> (n_uword_bits - 1)));
        } else {
            /* K X
             * ---
             * K K
             */
            sr = __builtin_clz(d.s.high) - __builtin_clz(n.s.high);
            /* 0 <= sr <= n_uword_bits - 1 or sr large */
            if (sr > n_uword_bits - 1) {
                if (rem)
                    *rem = n.all;
                return 0;
            }
            ++sr;
            /* 1 <= sr <= n_uword_bits */
            /*  q.all = n.all << (n_udword_bits - sr); */
            q.s.low = 0;
            q.s.high = n.s.low << (n_uword_bits - sr);
            /* r.all = n.all >> sr;
             * if (sr < n_uword_bits)
             * {
             *     r.s.high = n.s.high >> sr;
             *     r.s.low = (n.s.high << (n_uword_bits - sr)) | (n.s.low >> sr);
             * }
             * else
             * {
             *     r.s.high = 0;
             *     r.s.low = n.s.high;
             * }
             */
            r.s.high = (n.s.high >> sr) &
                       ((si_int) (sr - n_uword_bits) >> (n_uword_bits - 1));
            r.s.low = (n.s.high << (n_uword_bits - sr)) |
                      ((n.s.low >> sr) &
                       ((si_int) (sr - n_uword_bits) >> (n_uword_bits - 1)));
        }
    }
    /* Not a special case
     * q and r are initialized with:
     * q.all = n.all << (n_udword_bits - sr);
     * r.all = n.all >> sr;
     * 1 <= sr <= n_udword_bits - 1
     */
    su_int carry = 0;
    for (; sr > 0; --sr) {
        /* r:q = ((r:q)  << 1) | carry */
        r.s.high = (r.s.high << 1) | (r.s.low >> (n_uword_bits - 1));
        r.s.low = (r.s.low << 1) | (q.s.high >> (n_uword_bits - 1));
        q.s.high = (q.s.high << 1) | (q.s.low >> (n_uword_bits - 1));
        q.s.low = (q.s.low << 1) | carry;
        /* carry = 0;
         * if (r.all >= d.all)
         * {
         *      r.all -= d.all;
         *      carry = 1;
         * }
         */
        const di_int s = (di_int) (d.all - r.all - 1) >> (n_udword_bits - 1);
        carry = s & 1;
        r.all -= d.all & s;
    }
    q.all = (q.all << 1) | carry;
    if (rem)
        *rem = r.all;
    return q.all;
}

du_int
__umoddi3(du_int a, du_int b) {
    du_int r;
    __udivmoddi4(a, b, &r);
    return r;
}

float _Complex
__mulsc3(float __a, float __b, float __c, float __d) {
    float __ac = __a * __c;
    float __bd = __b * __d;
    float __ad = __a * __d;
    float __bc = __b * __c;
    float _Complex
    z;
    __real__ z = __ac - __bd;
    __imag__ z = __ad + __bc;
    if (isnan(__real__ z) && isnan(__imag__ z)) {
        int __recalc = 0;
        if (isinf(__a) || isinf(__b)) {
            __a = copysignf(isinf(__a) ? 1 : 0, __a);
            __b = copysignf(isinf(__b) ? 1 : 0, __b);
            if (isnan(__c))
                __c = copysignf(0, __c);
            if (isnan(__d))
                __d = copysignf(0, __d);
            __recalc = 1;
        }
        if (isinf(__c) || isinf(__d)) {
            __c = copysignf(isinf(__c) ? 1 : 0, __c);
            __d = copysignf(isinf(__d) ? 1 : 0, __d);
            if (isnan(__a))
                __a = copysignf(0, __a);
            if (isnan(__b))
                __b = copysignf(0, __b);
            __recalc = 1;
        }
        if (!__recalc && (isinf(__ac) || isinf(__bd) ||
                          isinf(__ad) || isinf(__bc))) {
            if (isnan(__a))
                __a = copysignf(0, __a);
            if (isnan(__b))
                __b = copysignf(0, __b);
            if (isnan(__c))
                __c = copysignf(0, __c);
            if (isnan(__d))
                __d = copysignf(0, __d);
            __recalc = 1;
        }
        if (__recalc) {
            __real__ z = INFINITY * (__a * __c - __b * __d);
            __imag__ z = INFINITY * (__a * __d + __b * __c);
        }
    }
    return z;
}

double _Complex
__muldc3(double __a, double __b, double __c, double __d) {
    double __ac = __a * __c;
    double __bd = __b * __d;
    double __ad = __a * __d;
    double __bc = __b * __c;
    double _Complex
    z;
    __real__ z = __ac - __bd;
    __imag__ z = __ad + __bc;
    if (isnan(__real__ z) && isnan(__imag__ z)) {
        int __recalc = 0;
        if (isinf(__a) || isinf(__b)) {
            __a = copysign(isinf(__a) ? 1 : 0, __a);
            __b = copysign(isinf(__b) ? 1 : 0, __b);
            if (isnan(__c))
                __c = copysign(0, __c);
            if (isnan(__d))
                __d = copysign(0, __d);
            __recalc = 1;
        }
        if (isinf(__c) || isinf(__d)) {
            __c = copysign(isinf(__c) ? 1 : 0, __c);
            __d = copysign(isinf(__d) ? 1 : 0, __d);
            if (isnan(__a))
                __a = copysign(0, __a);
            if (isnan(__b))
                __b = copysign(0, __b);
            __recalc = 1;
        }
        if (!__recalc && (isinf(__ac) || isinf(__bd) ||
                          isinf(__ad) || isinf(__bc))) {
            if (isnan(__a))
                __a = copysign(0, __a);
            if (isnan(__b))
                __b = copysign(0, __b);
            if (isnan(__c))
                __c = copysign(0, __c);
            if (isnan(__d))
                __d = copysign(0, __d);
            __recalc = 1;
        }
        if (__recalc) {
            __real__ z = INFINITY * (__a * __c - __b * __d);
            __imag__ z = INFINITY * (__a * __d + __b * __c);
        }
    }
    return z;
}

du_int
__udivdi3(du_int a, du_int b) {
    return __udivmoddi4(a, b, 0);
}

di_int
__moddi3(di_int a, di_int b) {
    const int bits_in_dword_m1 = (int) (sizeof(di_int) * CHAR_BIT) - 1;
    di_int s = b >> bits_in_dword_m1;  /* s = b < 0 ? -1 : 0 */
    b = (b ^ s) - s;                   /* negate if s == -1 */
    s = a >> bits_in_dword_m1;         /* s = a < 0 ? -1 : 0 */
    a = (a ^ s) - s;                   /* negate if s == -1 */
    di_int r;
    __udivmoddi4(a, b, (du_int *) &r);
    return (r ^ s) - s;                /* negate if s == -1 */
}

di_int
__divdi3(di_int a, di_int b) {
    const int bits_in_dword_m1 = (int) (sizeof(di_int) * CHAR_BIT) - 1;
    di_int s_a = a >> bits_in_dword_m1;           /* s_a = a < 0 ? -1 : 0 */
    di_int s_b = b >> bits_in_dword_m1;           /* s_b = b < 0 ? -1 : 0 */
    a = (a ^ s_a) - s_a;                         /* negate if s_a == -1 */
    b = (b ^ s_b) - s_b;                         /* negate if s_b == -1 */
    s_a ^= s_b;                                  /*sign of quotient */
    return (__udivmoddi4(a, b, (du_int *) 0) ^ s_a) - s_a;  /* negate if s_a == -1 */
}

#ifndef __SOFT_FP__
/* Support for systems that have hardware floating-point; we'll set the inexact flag
 * as a side-effect of this computation.
 */
double
__floatdidf(di_int a) {
    static const double twop52 = 0x1.0p52;
    static const double twop32 = 0x1.0p32;

    union {
        int64_t x;
        double d;
    } low = {.d = twop52};

    const double high = (int32_t)(a >> 32) * twop32;
    low.x |= a & INT64_C(0x00000000ffffffff);

    const double result = (high - twop52) + low.d;
    return result;
}

#else
/* Support for systems that don't have hardware floating-point; there are no flags to
 * set, and we don't want to code-gen to an unknown soft-float implementation.
 */
double
__floatdidf(di_int a) {
    if (a == 0)
        return 0.0;
    const unsigned N = sizeof(di_int) * CHAR_BIT;
    const di_int s = a >> (N-1);
    a = (a ^ s) - s;
    int sd = N - clzll(a);  /* number of significant digits */
    int e = sd - 1;             /* exponent */
    if (sd > DBL_MANT_DIG) {
        /*  start:  0000000000000000000001xxxxxxxxxxxxxxxxxxxxxxPQxxxxxxxxxxxxxxxxxx
         *  finish: 000000000000000000000000000000000000001xxxxxxxxxxxxxxxxxxxxxxPQR
         *                                                12345678901234567890123456
         *  1 = msb 1 bit
         *  P = bit DBL_MANT_DIG-1 bits to the right of 1
         * Q = bit DBL_MANT_DIG bits to the right of 1
         *  R = "or" of all bits to the right of Q
        */
        switch (sd)
        {
        case DBL_MANT_DIG + 1:
            a <<= 1;
            break;
        case DBL_MANT_DIG + 2:
            break;
        default:
            a = ((du_int)a >> (sd - (DBL_MANT_DIG+2))) |
                ((a & ((du_int)(-1) >> ((N + DBL_MANT_DIG+2) - sd))) != 0);
        };
        /* finish: */
        a |= (a & 4) != 0;  /* Or P into R */
        ++a;  /* round - this step may add a significant bit */
        a >>= 2;  /* dump Q and R */
        /* a is now rounded to DBL_MANT_DIG or DBL_MANT_DIG+1 bits */
        if (a & ((du_int)1 << DBL_MANT_DIG)) {
            a >>= 1;
            ++e;
        }
        /* a is now rounded to DBL_MANT_DIG bits */
    }
    else {
        a <<= (DBL_MANT_DIG - sd);
        /* a is now rounded to DBL_MANT_DIG bits */
    }
    double_bits fb;
    fb.u.high = ((su_int)s & 0x80000000) |        /* sign */
                ((e + 1023) << 20)      |         /* exponent */
                ((su_int)(a >> 32) & 0x000FFFFF); /* mantissa-high */
    fb.u.low = (su_int)a;                         /* mantissa-low */
    return fb.f;
}
#endif