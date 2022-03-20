/*
 * $Id: complex_casinh.c,v 1.0 2021-01-30 11:33:52 clib2devs Exp $
*/

#ifndef _COMPLEX_HEADERS_H
#include "complex_headers.h"
#endif /* _COMPLEX_HEADERS_H */

double complex

casinh(double complex z) {
    double complex zp = casin(CMPLX(-cimag(z), creal(z)));
    return CMPLX(cimag(zp), -creal(zp));
}
