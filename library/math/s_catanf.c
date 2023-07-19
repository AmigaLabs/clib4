/*
 * $Id: math_s_catanf.c,v 1.1 2023-07-19 11:33:52 clib2devs Exp $
 */

#ifndef _COMPLEX_HEADERS_H
#include "complex_headers.h"
#endif /* _COMPLEX_HEADERS_H */

#define MAXNUMF 1.0e38F

static const double DP1 = 3.140625;
static const double DP2 = 9.67502593994140625E-4;
static const double DP3 = 1.509957990978376432E-7;

static float
_redupif(float xx) {
    float x, t;
    long i;

    x = xx;
    t = x / (float) M_PI;
    if (t >= 0.0)
        t += 0.5;
    else
        t -= 0.5;

    i = t;    /* the multiple */
    t = i;
    t = ((x - t * DP1) - t * DP2) - t * DP3;
    return (t);
}

float complex
catanf(float complex z) {
    float complex w;
    float a, t, x, x2, y;

    x = crealf(z);
    y = cimagf(z);

    if ((x == 0.0f) && (y > 1.0f))
        goto ovrf;

    x2 = x * x;
    a = 1.0f - x2 - (y * y);
    if (a == 0.0f)
        goto ovrf;

    t = 0.5f * atan2f(2.0f * x, a);
    w = _redupif(t);

    t = y - 1.0f;
    a = x2 + (t * t);
    if (a == 0.0f)
        goto ovrf;

    t = y + 1.0f;
    a = (x2 + (t * t)) / a;
    w = w + (0.25f * logf(a)) * I;
    return (w);

ovrf:
    /*mtherr( "catanf", OVERFLOW );*/
    w = MAXNUMF + MAXNUMF * I;
    return (w);
}