/*
 * $Id: math_s_cacosl.c,v 1.1 2023-07-19 11:33:52 clib2devs Exp $
*/

#ifndef _COMPLEX_HEADERS_H
#include "complex_headers.h"
#endif /* _COMPLEX_HEADERS_H */

static const long double PIO2L = 1.570796326794896619231321691639751442098585L;

long double complex
cacosl(long double complex z) {
    long double complex w;

    w = casinl(z);
    w = (PIO2L - creall(w)) - cimagl(w) * I;
    return (w);
}
