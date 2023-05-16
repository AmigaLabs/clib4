/*
 * $Id: math_signbit.c,v 2.0 2023-06-12 12:04:24 clib2devs Exp $
 */

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

int
__signbit_double(double x) {
    union {
        double d;
        uint64_t i;
    } y = {x};
    return y.i >> 63;
}

int
__signbit_float(float x) {
    union {
        float f;
        uint32_t i;
    } y = {x};
    return y.i >> 31;
}

int
__signbit_long_double(long double x) {
    return __signbit_double(x);
}