/*
 * $Id: math_s_casinhl.c,v 1.1 2023-07-19 11:33:52 clib2devs Exp $
*/

#ifndef _COMPLEX_HEADERS_H
#include "complex_headers.h"
#endif /* _COMPLEX_HEADERS_H */

long double complex
casinhl(long double complex z) {
    long double complex w;

    w = -1.0L * I * casinl(z * I);
    return (w);
}
