/*
 * $Id: math_llrint.c,v 1.2 2022-03-13 10:09:48 clib2devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

long long
llrint(double x) {
    fenv_t env;
    long long d;

    feholdexcept(&env);
    d = (long long) rint(x);
    if (fetestexcept(FE_INVALID))
        feclearexcept(FE_INEXACT);
    feupdateenv(&env);
    return (d);
}
