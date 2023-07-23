/*
 * $Id: math_s_fpclassify.c,v 2.0 2023-07-19 12:04:23 clib2devs Exp $
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
    uint32_t msw, lsw;

    EXTRACT_WORDS(msw, lsw, d);

    if ((msw == 0x00000000 && lsw == 0x00000000) || (msw == 0x80000000 && lsw == 0x00000000))
        return FP_ZERO;
    else if ((msw >= 0x00100000 && msw <= 0x7fefffff) || (msw >= 0x80100000 && msw <= 0xffefffff))
        return FP_NORMAL;
    else if ((msw >= 0x00000000 && msw <= 0x000fffff) || (msw >= 0x80000000 && msw <= 0x800fffff))
        /* zero is already handled above */
        return FP_SUBNORMAL;
    else if ((msw == 0x7ff00000 && lsw == 0x00000000) || (msw == 0xfff00000 && lsw == 0x00000000))
        return FP_INFINITE;
    else
        return FP_NAN;
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