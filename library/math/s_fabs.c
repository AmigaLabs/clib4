/*
 * $Id: math_fabs.c,v 1.10 2023-07-13 12:04:23 clib4devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

#if defined(__arm__)
/* ARM: the compiler's own fabs, a bit operation on the sign. No asm needed. */
double
fabs(double x) {
    return __builtin_fabs(x);
}
#elif !defined(__SPE__)
inline static double
__fabs(double x) {
    double res;

    __asm volatile("fabs %0, %1"
            : "=f"(res)
            : "f"(x));

    return res;
}

double
fabs(double x) {
    return __fabs(x);
}
#endif
