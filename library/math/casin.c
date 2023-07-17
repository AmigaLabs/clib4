/*
 * $Id: complex_casin.c,v 1.0 2021-01-30 11:33:52 clib2devs Exp $
*/

#ifndef _COMPLEX_HEADERS_H
#include "complex_headers.h"
#endif /* _COMPLEX_HEADERS_H */

double complex
casin(double complex z) {
    double x = creal(z), y = cimag(z);
    double complex
            w = CMPLX((double) 1.0 - (x - y) * (x + y), (double) -2.0 * x * y),
            r = clog(CMPLX(-y, x) + csqrt(w));
    return CMPLX(cimag(r), -creal(r));
}
