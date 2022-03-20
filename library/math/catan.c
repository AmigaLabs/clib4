/*
 * $Id: complex_catan.c,v 1.0 2021-01-30 11:33:52 clib2devs Exp $
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

static double
_redupi(double x) {
    double t = x / M_PI, dp1 = 3.14159265160560607910E0,
            dp2 = 1.98418714791870343106E-9, dp3 = 1.14423774522196636802E-17;
    long int i = t >= 0.0f ? t + 0.5f : t - 0.5f;

    t = i;
    return ((x - t * dp1) - t * dp2) - t * dp3;
}

double complex
catan(double complex z) {
    double x = creal(z), y = cimag(z), x2 = x * x, a = 1.0 - x2 - (y * y),
            t = 0.5 * atan2(2.0 * x, a), w = _redupi(t);

    t = y - 1.0;
    a = x2 + (t * t);
    t = y + 1.0;
    a = (x2 + t * t) / a;
    return CMPLX(w, 0.25 * log(a));
}
