/*
 * $Id: complex_casinf.c,v 1.0 2021-01-30 11:33:52 clib2devs Exp $
*/

#ifndef _COMPLEX_HEADERS_H
#include "complex_headers.h"
#endif /* _COMPLEX_HEADERS_H */

float complex

casinf(float complex z) {
    float x = crealf(z), y = cimagf(z);
    float complex
            w = CMPLXF(1.0f - (x - y) * (x + y), -2.0f * x * y),
            r = clogf(CMPLX(-y, x) + csqrtf(w));
    return CMPLXF(cimagf(r), -crealf(r));
}
