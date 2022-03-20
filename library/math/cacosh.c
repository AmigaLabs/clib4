/*
 * $Id: complex_cacosh.c,v 1.0 2021-01-30 11:33:52 clib2devs Exp $
*/

#ifndef _COMPLEX_HEADERS_H
#include "complex_headers.h"
#endif /* _COMPLEX_HEADERS_H */

double complex

cacosh(double complex z) {
    double complex
    zp = cacos(z);
    return signbit(cimag(z)) ? CMPLX(cimag(zp), -creal(zp))
                             : CMPLX(-cimag(zp), creal(zp));
}
