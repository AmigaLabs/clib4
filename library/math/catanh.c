/*
 * $Id: complex_catanh.c,v 1.0 2021-01-30 11:33:52 clib2devs Exp $
*/

#ifndef _COMPLEX_HEADERS_H
#include "complex_headers.h"
#endif /* _COMPLEX_HEADERS_H */

double complex

catanh(double complex z) {
    double complex zp = catan(CMPLX(-cimag(z), creal(z)));
    return CMPLX(cimag(zp), -creal(zp));
}
