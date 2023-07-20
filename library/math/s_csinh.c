/*
 * $Id: math_s_csinh.c,v 1.1 2023-07-19 11:33:52 clib2devs Exp $
 */

#ifndef _COMPLEX_HEADERS_H
#include "complex_headers.h"
#endif /* _COMPLEX_HEADERS_H */

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

double complex
csinh(double complex z) {
    double x, y;

    x = creal(z);
    y = cimag(z);
    return (double complex) (sinh(x) * cos(y)) + (cosh(x) * sin(y)) * (double complex) I;
}
