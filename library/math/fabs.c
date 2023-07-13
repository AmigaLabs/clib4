/*
 * $Id: math_fabs.c,v 1.10 2023-07-13 12:04:23 clib2devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

inline static double
__fabs(double x) {
#ifndef _SOFT_FLOAT
    double res;

    __asm volatile("fabs %0, %1"
                   : "=f"(res)
                   : "f"(x));

    return res;
#else
    uint32_t high;
    GET_HIGH_WORD(high, x);
    SET_HIGH_WORD(x, high & 0x7fffffff);

    return x;
#endif
}

double
fabs(double x) {
    double result;

    result = __fabs(x);

    return (result);
}
