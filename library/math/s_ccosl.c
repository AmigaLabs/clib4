/*
 * $Id: math_s_ccosl.c,v 1.1 2023-07-19 11:33:52 clib2devs Exp $
*/

#ifndef _COMPLEX_HEADERS_H
#include "complex_headers.h"
#endif /* _COMPLEX_HEADERS_H */

static void
cchshl(long double x, long double *c, long double *s) {
    long double e, ei;

    if (fabsl(x) <= 0.5L) {
        *c = coshl(x);
        *s = sinhl(x);
    } else {
        e = expl(x);
        ei = 0.5L / e;
        e = 0.5L * e;
        *s = e - ei;
        *c = e + ei;
    }
}

long double complex

ccosl(long double complex z) {
    long double complex w;
    long double ch, sh;

    cchshl(cimagl(z), &ch, &sh);
    w = cosl(creall(z)) * ch + (-sinl(creall(z)) * sh) * I;
    return (w);
}
