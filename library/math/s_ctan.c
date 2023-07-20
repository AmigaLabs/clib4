/*
 * $Id: math_s_ctan.c,v 1.1 2023-07-19 11:33:52 clib2devs Exp $
*/

#ifndef _COMPLEX_HEADERS_H
#include "complex_headers.h"
#endif /* _COMPLEX_HEADERS_H */

#define MACHEP 1.1e-16

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

/*  Taylor series expansion for cosh(2y) - cos(2x)	*/
static double
_ctans(double complex z) {
    double f, x, x2, y, y2, rn, t;
    double d;

    x = fabs(2.0 * creal(z));
    y = fabs(2.0 * cimag(z));

    x = _redupi(x);

    x = x * x;
    y = y * y;
    x2 = 1.0;
    y2 = 1.0;
    f = 1.0;
    rn = 0.0;
    d = 0.0;
    do {
        rn += 1.0;
        f *= rn;
        rn += 1.0;
        f *= rn;
        x2 *= x;
        y2 *= y;
        t = y2 + x2;
        t /= f;
        d += t;

        rn += 1.0;
        f *= rn;
        rn += 1.0;
        f *= rn;
        x2 *= x;
        y2 *= y;
        t = y2 - x2;
        t /= f;
        d += t;
    } while (fabs(t / d) > MACHEP);
    return (d);
}

double complex
ctan(double complex z) {
    double complex w;
    double d;

    d = cos(2.0 * creal(z)) + cosh(2.0 * cimag(z));

    if (fabs(d) < 0.25)
        d = _ctans(z);

    if (d == 0.0) {
        /*mtherr ("ctan", OVERFLOW);*/
        w = HUGE_VAL + HUGE_VAL * I;
        return (w);
    }

    w = sin(2.0 * creal(z)) / d + (sinh(2.0 * cimag(z)) / d) * I;
    return (w);
}
