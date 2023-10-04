/*
 * $Id: math_s_cimagl.c,v 1.0 2021-01-30 11:33:52 clib4devs Exp $
*/

#ifndef _COMPLEX_HEADERS_H
#include "complex_headers.h"
#endif /* _COMPLEX_HEADERS_H */

long double
cimagl(long double complex z) {
    return (IMAG(z));
}