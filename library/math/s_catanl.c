/*
 * $Id: math_s_catanl.c,v 1.1 2023-07-19 11:33:52 clib2devs Exp $
*/

#ifndef _COMPLEX_HEADERS_H
#include "complex_headers.h"
#endif /* _COMPLEX_HEADERS_H */

static const long double PIL = 3.141592653589793238462643383279502884197169L;
static const long double DP1 = 3.14159265358979323829596852490908531763125L;
static const long double DP2 = 1.6667485837041756656403424829301998703007e-19L;
static const long double DP3 = 1.8830410776607851167459095484560349402753e-39L;

static long double
redupil(long double x) {
    long double t;
    long i;

    t = x / PIL;
    if (t >= 0.0L)
        t += 0.5L;
    else
        t -= 0.5L;

    i = t;    /* the multiple */
    t = i;
    t = ((x - t * DP1) - t * DP2) - t * DP3;
    return (t);
}

long double complex
catanl(long double complex z) {
    long double complex w;
    long double a, t, x, x2, y;

    x = creall(z);
    y = cimagl(z);

    if ((x == 0.0L) && (y > 1.0L))
        goto ovrf;

    x2 = x * x;
    a = 1.0L - x2 - (y * y);
    if (a == 0.0L)
        goto ovrf;

    t = atan2l(2.0L * x, a) * 0.5L;
    w = redupil(t);

    t = y - 1.0L;
    a = x2 + (t * t);
    if (a == 0.0L)
        goto ovrf;

    t = y + 1.0L;
    a = (x2 + (t * t)) / a;
    w = w + (0.25L * logl(a)) * I;
    return (w);

ovrf:
    /*mtherr( "catanl", OVERFLOW );*/
    w = LDBL_MAX + LDBL_MAX * I;
    return (w);
}

