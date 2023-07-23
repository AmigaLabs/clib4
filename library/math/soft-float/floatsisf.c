/*
 * $Id: math_soft-float_floatsisf.c,v 1.0 2023-07-18 07:54:24 clib2devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

SFtype
__floatsisf(SItype i) {
    FP_DECL_EX;
    FP_DECL_S(A);
    SFtype a;

    FP_INIT_ROUNDMODE;
    FP_FROM_INT_S(A, i, SI_BITS, USItype);
    FP_PACK_RAW_S(a, A);
    FP_HANDLE_EXCEPTIONS;

    return a;
}