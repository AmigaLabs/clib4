/*
 * $Id: math_s_copysignf.c,v 1.2 2023-07-19 12:04:23 clib2devs Exp $
 */

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

float
copysignf(float x, float y) {
    uint32_t ix, iy;

    GET_FLOAT_WORD(ix, x);
    GET_FLOAT_WORD(iy, y);
    SET_FLOAT_WORD(x, (ix & 0x7fffffff) | (iy & 0x80000000U));

    return x;
}
