/*
 * $Id: math_s_ccoshl.c,v 1.1 2023-07-19 11:33:52 clib4devs Exp $
*/

#ifndef _COMPLEX_HEADERS_H
#include "complex_headers.h"
#endif /* _COMPLEX_HEADERS_H */

long double complex
ccoshl(long double complex z) {
    long double complex w;
    long double x, y;

    x = creall(z);
    y = cimagl(z);
    w = coshl(x) * cosl(y) + (sinhl(x) * sinl(y)) * I;
    return (w);
}
