/*
 * $Id: math_s_ctanhl.c,v 1.1 2023-07-19 11:33:52 clib4devs Exp $
*/

#ifndef _COMPLEX_HEADERS_H
#include "complex_headers.h"
#endif /* _COMPLEX_HEADERS_H */

long double complex
ctanhl(long double complex z) {
    long double complex w;
    long double x, y, d;

    x = creall(z);
    y = cimagl(z);
    d = coshl(2.0L * x) + cosl(2.0L * y);
    w = sinhl(2.0L * x) / d + (sinl(2.0L * y) / d) * I;
    return (w);
}
