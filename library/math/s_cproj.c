/*
 * $Id: math_s_cproj.c,v 1.1 2023-07-19 11:33:52 clib2devs Exp $
*/

#ifndef _COMPLEX_HEADERS_H
#include "complex_headers.h"
#endif /* _COMPLEX_HEADERS_H */

double complex
cproj(double complex z) {
    if (!isinf(creal(z)) && !isinf(cimag(z)))
        return (z);
    else
        return (CMPLX(INFINITY, copysign(0.0, cimag(z))));
}
