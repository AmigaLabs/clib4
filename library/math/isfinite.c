/*
 * $Id: math_isfinite.c,v 1.3 2022-03-10 12:04:23 clib2devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

int
__isfinite_float(float x) {
    int32_t ix;
    GET_FLOAT_WORD(ix, x);
    ix &= 0x7fffffff;
    return (FLT_UWORD_IS_FINITE(ix));
}

int
__isfinite_double(double d) {
    union IEEEd2bits u;

    u.d = d;
    return (u.bits.exp != LDBL_INF_NAN_EXP);
}

int
__isfinite_long_double(long double d) {
    union IEEEd2bits u;

    u.d = d;
    return (u.bits.exp != LDBL_INF_NAN_EXP);
}