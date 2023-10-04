/*
 * $Id: math_s_ccosh.c,v 1.1 2023-07-19 11:33:52 clib4devs Exp $
 */

#ifndef _COMPLEX_HEADERS_H
#include "complex_headers.h"
#endif /* _COMPLEX_HEADERS_H */

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

double complex
ccosh(double complex z) {
    double x, y;

    x = creal(z);
    y = cimag(z);
    return cosh(x) * cos(y) * (double complex) 1.0 + (sinh(x) * sin(y)) * (double complex) I;
}
