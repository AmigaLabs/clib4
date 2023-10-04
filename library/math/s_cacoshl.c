/*
 * $Id: math_s_cacoshl.c,v 1.1 2023-07-19 11:33:52 clib4devs Exp $
*/

#ifndef _COMPLEX_HEADERS_H
#include "complex_headers.h"
#endif /* _COMPLEX_HEADERS_H */

long double complex
cacoshl(long double complex z) {
    long double complex w;

    w = clogl(z + csqrtl(z + 1) * csqrtl(z - 1));
    return (w);
}
