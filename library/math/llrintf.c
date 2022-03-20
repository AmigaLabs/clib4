/*
 * $Id: math_llrintf.c,v 1.0 2022-03-11 12:04:23 clib2devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

long long
llrintf(float x)
{
    fenv_t env;
    long long d;

    feholdexcept(&env);
    d = (long long) rintf(x);
    if (fetestexcept(FE_INVALID))
        feclearexcept(FE_INEXACT);
    feupdateenv(&env);
    return (d);
}
