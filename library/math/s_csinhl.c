/*
 * $Id: math_s_csinhl.c,v 1.2 2023-07-19 11:33:52 clib2devs Exp $
*/

#ifndef _COMPLEX_HEADERS_H
#include "complex_headers.h"
#endif /* _COMPLEX_HEADERS_H */

long double complex
csinhl(long double complex z) {
    long double complex w;
    long double x, y;

    x = creall(z);
    y = cimagl(z);
    w = sinhl(x) * cosl(y) + (coshl(x) * sinl(y)) * I;
    return (w);
}
