/*
 * $Id: math_isnan.c,v 1.1 2022-03-10 16:47:23 clib2devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

int
__isnan(double d) {
    int32_t hx, lx;
    EXTRACT_WORDS(hx, lx, d);
    hx &= 0x7fffffff;
    hx |= (uint32_t)(lx | (-lx)) >> 31;
    hx = 0x7ff00000 - hx;
    return (int) (((uint32_t) hx) >> 31);
}

int
__isnanf(float f) {
    int32_t ix;
    GET_FLOAT_WORD(ix, f);
    ix &= 0x7fffffff;
    ix = 0x7f800000 - ix;
    return (int) (((uint32_t)(ix)) >> 31);
}

int
__isnanl(long double e) {
    int32_t se, hx, lx;
    GET_LDOUBLE_WORDS(se, hx, lx, e);
    se = (se & 0x7fff) << 1;
    lx |= hx & 0x7fffffff;
    se |= (uint32_t)(lx | (-lx)) >> 31;
    se = 0xfffe - se;
    return (int) (((uint32_t)(se)) >> 31);
}