/*
 * $Id: math_s_catan.c,v 1.1 2023-07-19 11:33:52 clib4devs Exp $
 */

#ifndef _COMPLEX_HEADERS_H
#include "complex_headers.h"
#endif /* _COMPLEX_HEADERS_H */

static const double DP1 = 3.14159265160560607910E0;
static const double DP2 = 1.98418714791870343106E-9;
static const double DP3 = 1.14423774522196636802E-17;

static double
_redupi(double x) {
    double t;
    long i;

    t = x / M_PI;
    if (t >= 0.0)
        t += 0.5;
    else
        t -= 0.5;

    i = t;    /* the multiple */
    t = i;
    t = ((x - t * DP1) - t * DP2) - t * DP3;
    return (t);
}

double complex
catan(double complex z) {
    double complex w;
    double a, t, x, x2, y;

    x = creal(z);
    y = cimag(z);

    if ((x == 0.0) && (y > 1.0))
        goto ovrf;

    x2 = x * x;
    a = 1.0 - x2 - (y * y);
    if (a == 0.0)
        goto ovrf;

    t = 0.5 * atan2(2.0 * x, a);
    w = _redupi(t);

    t = y - 1.0;
    a = x2 + (t * t);
    if (a == 0.0)
        goto ovrf;

    t = y + 1.0;
    a = (x2 + (t * t)) / a;
    w = w + (0.25 * log(a)) * I;
    return (w);

ovrf:
    /*mtherr ("catan", OVERFLOW);*/
    w = HUGE_VAL + HUGE_VAL * I;
    return (w);
}
