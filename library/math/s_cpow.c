/*
 * $Id: math_s_cpow.c,v 1.0 2023-07-19 11:33:52 clib4devs Exp $
*/

#ifndef _COMPLEX_HEADERS_H
#include "complex_headers.h"
#endif /* _COMPLEX_HEADERS_H */

double complex
cpow(double complex a, double complex z) {
    double complex w;
    double x, y, r, theta, absa, arga;

    x = creal(z);
    y = cimag(z);
    absa = cabs(a);
    if (absa == (double) 0.0) {
        return ((double) 0.0 + (double) 0.0 * (double complex) I);
    }
    arga = carg(a);
    r = pow(absa, x);
    theta = x * arga;
    if (y != (double) 0.0) {
        r = r * exp(-y * arga);
        theta = theta + y * log(absa);
    }
    w = r * cos(theta) + (r * sin(theta)) * (double complex) I;
    return w;
}
