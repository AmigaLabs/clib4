/*
 * $Id: math_signbit.c,v 1.2 2006-01-08 12:04:24 clib2devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

int
__signbit_float(float f) {
    union IEEEf2bits u;

    u.f = f;
    return (u.bits.sign);
}

int
__signbit_double(double d) {
    union IEEEd2bits u;

    u.d = d;
    return (u.bits.sign);
}

int
__signbit_long_double(long double e) {
    union IEEEl2bits u;

    u.e = e;
    return (u.bits.sign);
}