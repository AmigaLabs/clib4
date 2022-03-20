/*
 * $Id: complex_cacos.c,v 1.0 2021-01-30 11:33:52 clib2devs Exp $
*/

#ifndef _COMPLEX_HEADERS_H
#include "complex_headers.h"
#endif /* _COMPLEX_HEADERS_H */

double complex
cacos(double complex z) {
    double complex
    zp = casin(z);
    return CMPLX(M_PI_2 - creal(zp), -cimag(zp));
}
