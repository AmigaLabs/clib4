/*
 * $Id: math_s_isnan.c,v 1.1 2023-07-19 16:47:23 clib2devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

int
__isnan(double d) {
#ifdef __SPE__
    int32_t hx, lx;
    EXTRACT_WORDS(hx, lx, d);
    hx &= 0x7fffffff;
    hx |= (uint32_t) (lx | (-lx)) >> 31;
    hx = 0x7ff00000 - hx;
    return (int) (((uint32_t) (hx)) >> 31);
#else
    union IEEEd2bits u;

    u.d = d;
    return (u.bits.exp == 2047 && (u.bits.manl != 0 || u.bits.manh != 0));
#endif
}

int
__isnanf(float f) {
#ifdef __SPE__
    int32_t ix;
	GET_FLOAT_WORD(ix, f);
	ix &= 0x7fffffff;
	return FLT_UWORD_IS_NAN(ix);
#else
    union IEEEf2bits u;

    u.f = f;
    return (u.bits.exp == 255 && u.bits.man != 0);
#endif
}

int
__isnanl(long double d) {
    return __isnan(d);
}