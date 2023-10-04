/*
 * $Id: math_s_ccos.c,v 1.1 2023-07-19 11:33:52 clib4devs Exp $
*/

#ifndef _COMPLEX_HEADERS_H
#include "complex_headers.h"
#endif /* _COMPLEX_HEADERS_H */

static void
_cchsh(double x, double *c, double *s) {
    double e, ei;

    if (fabs(x) <= 0.5) {
        *c = cosh(x);
        *s = sinh(x);
    } else {
        e = exp(x);
        ei = 0.5 / e;
        e = 0.5 * e;
        *s = e - ei;
        *c = e + ei;
    }
}

double complex
ccos(double complex z) {
    double complex w;
    double ch, sh;

    _cchsh(cimag(z), &ch, &sh);
    w = cos(creal(z)) * ch - (sin(creal(z)) * sh) * I;
    return (w);
}