/*
 * $Id: math_fabs.c,v 1.10 2023-07-13 12:04:23 clib4devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

#ifndef __SPE__
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
