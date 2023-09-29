/*
 * $Id: math_s_lrintf.c,v 1.4 2023-07-19 12:04:23 clib4devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

#ifndef __SPE__
long int
lrintf(float x) {
    fenv_t env;
    long d;

    feholdexcept(&env);
    d = (long) rintf(x);
    if (fetestexcept(FE_INVALID))
        feclearexcept(FE_INEXACT);
    feupdateenv(&env);
    return (d);
}
#endif