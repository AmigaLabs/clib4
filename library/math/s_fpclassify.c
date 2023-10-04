/*
 * $Id: math_s_fpclassify.c,v 2.0 2023-07-19 12:04:23 clib4devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

int
__fpclassify_float(float f) {
#ifdef __SPE__
    uint32_t w;

    GET_FLOAT_WORD(w, f);

    if (w == 0x00000000 || w == 0x80000000)
        return FP_ZERO;
    else if ((w >= 0x00800000 && w <= 0x7f7fffff) || (w >= 0x80800000 && w <= 0xff7fffff))
        return FP_NORMAL;
    else if ((w >= 0x00000001 && w <= 0x007fffff) || (w >= 0x80000001 && w <= 0x807fffff))
        return FP_SUBNORMAL;
    else if (w == 0x7f800000 || w == 0xff800000)
        return FP_INFINITE;
    else
        return FP_NAN;
#else
    union IEEEf2bits u;

    u.f = f;
    if (u.bits.exp == 255) {
        if (u.bits.man == 0) {
            return FP_INFINITE;
        } else {
            return FP_NAN;
        }
    } else if (u.bits.exp != 0) {
        return FP_NORMAL;
    } else if (u.bits.man == 0) {
        return FP_ZERO;
    } else {
        return FP_SUBNORMAL;
    }
#endif
}

int
__fpclassify_double(double d) {
#ifdef __SPE__
    union ieee_double x;
    int result;

    x.value = d;

    D(("number = 0x%08lx%08lx",x.raw[0],x.raw[1]));

    if(((x.raw[0] & 0x7ff00000) == 0x7ff00000) && ((x.raw[0] & 0x000fffff) != 0 || (x.raw[1] != 0))) {
        SHOWMSG("not a number");

        /* Exponent = 2047 and fraction != 0.0 -> not a number */
        result = FP_NAN;
    }
    else if (((x.raw[0] & 0x7fffffff) == 0x7ff00000) && (x.raw[1] == 0)) {
        SHOWMSG("infinity");

        /* Exponent = 2047 and fraction = 0.0 -> infinity */
        result = FP_INFINITE;
    }
    else if ((((x.raw[0] & 0x7fffffff) == 0) && (x.raw[1] == 0))) {
        SHOWMSG("zero");

        /* Both exponent and fraction are zero -> zero */
        result = FP_ZERO;
    }
    else if ((x.raw[0] & 0x7fff0000) == 0) {
        SHOWMSG("subnormal");

        /* Exponent = 0 -> subnormal (IEEE 754) */
        result = FP_SUBNORMAL;
    }
    else {
        SHOWMSG("normal");

        result = FP_NORMAL;
    }
    SHOWVALUE(result);

	return result;
#else
    union IEEEd2bits u;

    u.d = d;
    if (u.bits.exp == 2047) {
        if (u.bits.manl == 0 && u.bits.manh == 0) {
            return FP_INFINITE;
        } else {
            return FP_NAN;
        }
    } else if (u.bits.exp != 0) {
        return FP_NORMAL;
    } else if (u.bits.manl == 0 && u.bits.manh == 0) {
        return FP_ZERO;
    } else {
        return FP_SUBNORMAL;
    }
#endif
}


int
__fpclassify_long_double(long double e) {
    return __fpclassify_double(e);
}