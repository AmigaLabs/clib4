/*
 * $Id: math_s_clogl.c,v 1.1 2023-07-19 11:33:52 clib4devs Exp $
*/

#ifndef _COMPLEX_HEADERS_H
#include "complex_headers.h"
#endif /* _COMPLEX_HEADERS_H */

long double complex
clogl(long double complex z) {
    long double complex w;
    long double p, rr;

    /*rr = sqrt(z->r * z->r  +  z->i * z->i);*/
    p = cabsl(z);
    p = logl(p);
    rr = atan2l(cimagl(z), creall(z));
    w = p + rr * I;
    return (w);
}
