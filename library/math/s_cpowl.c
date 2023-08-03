/*
 * $Id: math_s_cpowl.c,v 1.1 2023-07-19 11:33:52 clib2devs Exp $
*/

#ifndef _COMPLEX_HEADERS_H
#include "complex_headers.h"
#endif /* _COMPLEX_HEADERS_H */

long double complex

cpowl(long double complex a, long double complex z) {
    long double complex w;
    long double x, y, r, theta, absa, arga;

    x = creall(z);
    y = cimagl(z);
    absa = cabsl(a);
    if (absa == 0.0L) {
        return (0.0L + 0.0L * I);
    }
    arga = cargl(a);
    r = powl(absa, x);
    theta = x * arga;
    if (y != 0.0L) {
        r = r * expl(-y * arga);
        theta = theta + y * logl(absa);
    }
    w = r * cosl(theta) + (r * sinl(theta)) * I;
    return (w);
}
