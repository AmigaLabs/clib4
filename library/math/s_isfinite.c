/*
 * $Id: math_s_isfinite.c,v 1.3 2023-07-19 12:04:23 clib2devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

int
__isfinite_float(float f) {
    union IEEEf2bits u;

    u.f = f;
    return (u.bits.exp != 255);
}

int
__isfinite_double(double d) {
    union IEEEd2bits u;

    u.d = d;
    return (u.bits.exp != 2047);
}

int
__isfinite_long_double(long double d) {
    return __isfinite_double(d);
}