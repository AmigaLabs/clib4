/*
 * $Id: complex_cproj.c,v 1.0 2021-01-30 11:33:52 clib2devs Exp $
*/

#ifndef _COMPLEX_HEADERS_H
#include "complex_headers.h"
#endif /* _COMPLEX_HEADERS_H */

double complex
cproj(double complex z) {
    return (isinf(creal(z)) || isinf(cimag(z))) ? CMPLX(INFINITY, copysign(0.0, creal(z))) : z;
}
