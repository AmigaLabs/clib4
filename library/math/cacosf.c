/*
 * $Id: complex_cacosf.c,v 1.0 2021-01-30 11:33:52 clib2devs Exp $
*/

#ifndef _COMPLEX_HEADERS_H
#include "complex_headers.h"
#endif /* _COMPLEX_HEADERS_H */

float complex
cacosf(float complex z) {
    float complex
    zp = casinf(z);
    return CMPLXF((float) M_PI_2 - crealf(zp), -cimagf(zp));
}
