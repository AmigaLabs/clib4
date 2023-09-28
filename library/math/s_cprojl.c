/*
 * $Id: math_s_cprojl.c,v 1.1 2023-07-19 11:33:52 clib4devs Exp $
*/

#ifndef _COMPLEX_HEADERS_H
#include "complex_headers.h"
#endif /* _COMPLEX_HEADERS_H */

long double complex
cprojl(long double complex z) {
    if (!isinf(creall(z)) && !isinf(cimagl(z)))
        return (z);
    else
        return (CMPLXL(INFINITY, copysignl(0.0, cimagl(z))));
}
