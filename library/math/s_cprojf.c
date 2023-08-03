/*
 * $Id: math_s_cprojf.c,v 1.1 2023-07-19 11:33:52 clib2devs Exp $
*/

#ifndef _COMPLEX_HEADERS_H
#include "complex_headers.h"
#endif /* _COMPLEX_HEADERS_H */

float complex
cprojf(float complex z) {
    if (!isinf(crealf(z)) && !isinf(cimagf(z)))
        return (z);
    else
        return (CMPLXF(INFINITY, copysignf(0.0, cimagf(z))));
}
