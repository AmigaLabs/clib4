/*
 * $Id: complex_cprojf.c,v 1.0 2021-01-30 11:33:52 clib2devs Exp $
*/

#ifndef _COMPLEX_HEADERS_H
#include "complex_headers.h"
#endif /* _COMPLEX_HEADERS_H */

float complex
cprojf(float complex z) {
    return (isinf(crealf(z)) || isinf(cimagf(z))) ? CMPLXF(INFINITY, copysignf(0.0, crealf(z))) : z;
}
