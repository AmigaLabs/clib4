/*
 * $Id: math_s_cexpl.c,v 1.1 2023-07-19 11:33:52 clib4devs Exp $
*/

#ifndef _COMPLEX_HEADERS_H
#include "complex_headers.h"
#endif /* _COMPLEX_HEADERS_H */

long double complex
cexpl(long double complex z) {
    long double complex w;
    long double r;

    r = expl(creall(z));
    w = r * cosl(cimagl(z)) + (r * sinl(cimagl(z))) * I;
    return (w);
}
