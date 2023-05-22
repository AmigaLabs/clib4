/*
 * $Id: math_fpclassify.c,v 2.0 2023-05-12 12:04:23 clib2devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

int
__fpclassify_float(float x) {
    union {
        float f;
        uint32_t i;
    } u = {x};
    int e = u.i >> 23 & 0xff;
    if (!e)
        return u.i << 1 ? FP_SUBNORMAL : FP_ZERO;
    if (e == 0xff)
        return u.i << 9 ? FP_NAN : FP_INFINITE;
    return FP_NORMAL;
}

int
__fpclassify_double(double x) {
    union {
        double f;
        uint64_t i;
    } u = {x};
    int e = u.i >> 52 & 0x7ff;
    if (!e)
        return u.i << 1 ? FP_SUBNORMAL : FP_ZERO;
    if (e == 0x7ff)
        return u.i << 12 ? FP_NAN : FP_INFINITE;
    return FP_NORMAL;
}


int
__fpclassify_long_double(long double x) {
    return __fpclassify_double(x);
}