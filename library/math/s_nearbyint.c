/*
 * $Id: math_s_nearbyint.c,v 1.4 2023-07-19 12:04:24 clib2devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

double
nearbyint(double x) {
    double ret;
    fenv_t env;

    fegetenv(&env);
    ret = rint(x);
    fesetenv(&env);

    return ret;
}
