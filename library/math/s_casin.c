/*
 * $Id: math_s_casin.c,v 1.1 2023-07-19 11:33:52 clib2devs Exp $
*/

#ifndef _COMPLEX_HEADERS_H
#include "complex_headers.h"
#endif /* _COMPLEX_HEADERS_H */

double complex
casin(double complex z) {
    double complex w;
    static double complex ca, ct, zz, z2;
    double x, y;

    x = creal(z);
    y = cimag(z);

    if (y == 0.0) {
        if (fabs(x) > 1.0) {
            w = M_PI_2 + 0.0 * I;
            /*mtherr ("casin", DOMAIN);*/
        } else {
            w = asin(x) + 0.0 * I;
        }
        return (w);
    }

    /* Power series expansion */
    /*
    b = cabs(z);
    if( b < 0.125 ) {
        z2.r = (x - y) * (x + y);
        z2.i = 2.0 * x * y;

        cn = 1.0;
        n = 1.0;
        ca.r = x;
        ca.i = y;
        sum.r = x;
        sum.i = y;
        do {
            ct.r = z2.r * ca.r  -  z2.i * ca.i;
            ct.i = z2.r * ca.i  +  z2.i * ca.r;
            ca.r = ct.r;
            ca.i = ct.i;

            cn *= n;
            n += 1.0;
            cn /= n;
            n += 1.0;
            b = cn/n;

            ct.r *= b;
            ct.i *= b;
            sum.r += ct.r;
            sum.i += ct.i;
            b = fabs(ct.r) + fabs(ct.i);
        }
        while( b > MACHEP );
        w->r = sum.r;
        w->i = sum.i;
        return;
    }
    */

    ca = x + y * I;
    ct = ca * I;
    /* sqrt( 1 - z*z) */
    /* cmul( &ca, &ca, &zz ) */
    /*x * x  -  y * y */
    zz = (x - y) * (x + y) + (2.0 * x * y) * I;

    zz = 1.0 - creal(zz) - cimag(zz) * I;
    z2 = csqrt(zz);

    zz = ct + z2;
    zz = clog(zz);
    /* multiply by 1/i = -i */
    w = zz * (-1.0 * I);
    return (w);
}
