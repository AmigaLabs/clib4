/*
 * $Id: math_s_ctanf.c,v 1.1 2023-07-19 11:33:52 clib2devs Exp $
*/

#ifndef _COMPLEX_HEADERS_H
#include "complex_headers.h"
#endif /* _COMPLEX_HEADERS_H */

#define MACHEPF 3.0e-8
#define MAXNUMF 1.0e38f

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

/*  Taylor series expansion for cosh(2y) - cos(2x)	*/
static float
_ctansf(float complex z) {
    float f, x, x2, y, y2, rn, t, d;

    x = fabsf(2.0f * crealf(z));
    y = fabsf(2.0f * cimagf(z));

    x = _redupif(x);

    x = x * x;
    y = y * y;
    x2 = 1.0f;
    y2 = 1.0f;
    f = 1.0f;
    rn = 0.0f;
    d = 0.0f;
    do {
        rn += 1.0f;
        f *= rn;
        rn += 1.0f;
        f *= rn;
        x2 *= x;
        y2 *= y;
        t = y2 + x2;
        t /= f;
        d += t;

        rn += 1.0f;
        f *= rn;
        rn += 1.0f;
        f *= rn;
        x2 *= x;
        y2 *= y;
        t = y2 - x2;
        t /= f;
        d += t;
    } while (fabsf(t / d) > MACHEPF);
    return (d);
}

float complex
ctanf(float complex z) {
    float complex w;
    float d;

    d = cosf(2.0f * crealf(z)) + coshf(2.0f * cimagf(z));

    if (fabsf(d) < 0.25f)
        d = _ctansf(z);

    if (d == 0.0f) {
        /*mtherr( "ctanf", OVERFLOW );*/
        w = MAXNUMF + MAXNUMF * I;
        return (w);
    }
    w = sinf(2.0f * crealf(z)) / d + (sinhf(2.0f * cimagf(z)) / d) * I;
    return (w);
}
