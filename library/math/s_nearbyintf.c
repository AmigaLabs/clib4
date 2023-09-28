/*
 * $Id: math_s_nearbyintf.c,v 1.4 2023-07-19 12:04:24 clib4devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

float
nearbyintf(float x) {
    float ret;
    fenv_t env;

    fegetenv(&env);
    ret = rintf(x);
    fesetenv(&env);

    return ret;
}
