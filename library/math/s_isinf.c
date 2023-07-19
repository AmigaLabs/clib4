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
    union IEEEd2bits u;

    u.d = d;
    return (u.bits.exp == 2047 && u.bits.manl == 0 && u.bits.manh == 0);
}

int
__isinff(float f) {
    union IEEEf2bits u;

    u.f = f;
    return (u.bits.exp == 255 && u.bits.man == 0);
}

int
__isinfl(long double e) {
    union IEEEl2bits u;

    u.e = e;
    mask_nbit_l(u);
    return (u.bits.exp == 2047 && u.bits.manl == 0 && u.bits.manh == 0);
}