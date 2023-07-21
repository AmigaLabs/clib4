/*
 * $Id: math_s_signbit.c,v 2.0 2023-07-19 12:04:24 clib2devs Exp $
 */

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

#ifndef __SPE__
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
#else
int
__signbit_double(double d) {
    return __builtin_signbit(d);
}

int
__signbit_float(float f) {
    return __builtin_signbitf(f);
}

int
__signbit_long_double(long double e) {
    return __builtin_signbitl(e);
}
#endif