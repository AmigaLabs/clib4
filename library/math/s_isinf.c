/*
 * $Id: math_s_isinf.c,v 1.1 2023-07-19 16:37:23 clib2devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

int
__isinf(double d) {
#ifndef __SPE__
    union IEEEd2bits u;

    u.d = d;
    return (u.bits.exp == 2047 && u.bits.manl == 0 && u.bits.manh == 0);
#else
    int32_t hx , lx;
    EXTRACT_WORDS(hx, lx, d);
    hx &= 0x7fffffff;
    hx |= (uint32_t) (lx | (-lx)) >> 31;
    hx = 0x7ff00000 - hx;
    return 1 - (int) ((uint32_t) (hx | (-hx)) >> 31);
#endif
}

int
__isinff(float f) {
#ifndef __SPE__
    union IEEEf2bits u;

    u.f = f;
    return (u.bits.exp == 255 && u.bits.man == 0);
#else
    int32_t ix;
    GET_FLOAT_WORD(ix, f);
    ix &= 0x7fffffff;
    return FLT_UWORD_IS_NAN(ix);
#endif
}

int
__isinfl(long double e) {
    return __isinf(e);
}