/*
 * $Id: math_s_isormal.c,v 1.1 2023-07-19 16:47:23 clib2devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

int
__isnormal(double d) {
    union IEEEd2bits u;

    u.d = d;
    return (u.bits.exp != 0 && u.bits.exp != 2047);
}

int
__isnormalf(float f) {
    union IEEEf2bits u;

    u.f = f;
    return (u.bits.exp != 0 && u.bits.exp != 255);
}

int
__isnormall(long double e) {
    union IEEEl2bits u;

    u.e = e;
    return (u.bits.exp != 0 && u.bits.exp != 2047);
}