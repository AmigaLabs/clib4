/*
 * $Id: complex_ctanh.c,v 1.0 2021-01-30 11:33:52 clib2devs Exp $
 */

#ifndef _COMPLEX_HEADERS_H
#include "complex_headers.h"
#endif /* _COMPLEX_HEADERS_H */

double complex

ctanh(double complex z) {
    double x = creal(z), y = cimag(z), d = cosh((double) 2.0 * x) + cos((double) 2.0 * y);
    return CMPLX(sinh((double) 2.0 * x) / d, sin((double) 2.0 * y) / d);
}
