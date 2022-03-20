/*
 * $Id: complex_ctanh.c,v 1.0 2021-01-30 11:33:52 clib2devs Exp $
 *

 *
 *****************************************************************************
 *
 * Documentation and source code for this library, and the most recent library
 * build are available from <https://github.com/afxgroup/clib2>.
 *
 *****************************************************************************
 * Imported and modified 2021/01/25 by Ola Soder <rolfkopman@gmail.com>
 */

#ifndef _COMPLEX_HEADERS_H
#include "complex_headers.h"
#endif /* _COMPLEX_HEADERS_H */

double complex

ctanh(double complex z) {
    double x = creal(z), y = cimag(z), d = cosh(2.0 * x) + cos(2.0 * y);
    return CMPLX(sinh(2.0 * x) / d, sin(2.0 * y) / d);
}
