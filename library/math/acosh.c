/*
 * $Id: math_acosh.c,v 1.3 2006-01-08 12:04:23 clib2devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

static const double
        one = 1.0,
        ln2 = 6.93147180559945286227e-01; /* 0x3FE62E42, 0xFEFA39EF */

inline static
double
__acosh(double x) {
    union {
        double f;
        uint64_t i;
    } u = {.f = x};
    unsigned e = u.i >> 52 & 0x7ff;

    /* x < 1 domain error is handled in the called functions */

    if (e < 0x3ff + 1)
        /* |x| < 2, up to 2ulp error in [1,1.125] */
        return log1p(x - 1 + sqrt((x - 1) * (x - 1) + 2 * (x - 1)));
    if (e < 0x3ff + 26)
        /* |x| < 0x1p26 */
        return log(2 * x - 1 / (x + sqrt(x * x - 1)));
    /* |x| >= 0x1p26 or nan */
    return log(x) + 0.693147180559945309417232121458176568;
}

double
acosh(double x) {
    double result;

    if (x < 1.0) {
        result = NAN;
        __set_errno(EDOM);
        feraiseexcept(FE_INVALID);
    } else {
        result = __acosh(x);
    }

    return (result);
}