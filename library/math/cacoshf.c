/*
 * $Id: complex_cacoshf.c,v 1.0 2021-01-30 11:33:52 clib2devs Exp $
*/
#ifndef _COMPLEX_HEADERS_H
#include "complex_headers.h"
#endif /* _COMPLEX_HEADERS_H */

float complex
cacoshf(float complex z) {
    float complex
    zp = cacosf(z);
    return signbit(cimagf(z)) ? CMPLXF(cimagf(zp), -crealf(zp))
                              : CMPLXF(-cimagf(zp), crealf(zp));
}
