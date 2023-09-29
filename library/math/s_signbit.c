/*
 * $Id: math_s_signbit.c,v 2.0 2023-07-19 12:04:24 clib4devs Exp $
 */

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

int
__signbit_double(double d) {
    union IEEEd2bits u;
    u.d = d;

    return (u.bits.sign);
}

int
__signbit_float(float f) {
    union IEEEf2bits u;

    u.f = f;
    return (u.bits.sign);
}

int
__signbit_long_double(long double e) {
    return __signbit_double(e);
}
