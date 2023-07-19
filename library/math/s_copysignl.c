/*
 * $Id: math_s_copysignl.c,v 1.2 2023-07-18 12:04:23 clib2devs Exp $
 */

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

long double
copysignl(long double x, long double y) {
    union IEEEl2bits ux, uy;

    ux.e = x;
    uy.e = y;
    ux.bits.sign = uy.bits.sign;
    return (ux.e);
}
