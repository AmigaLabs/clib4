/*
 * $Id: math_soft-float_fixunssfsi.c,v 1.0 2023-07-18 07:54:24 clib2devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

USItype
__fixunssfsi(SFtype a) {
    FP_DECL_EX;
    FP_DECL_S(A);
    USItype r;

    FP_INIT_EXCEPTIONS;
    FP_UNPACK_RAW_S(A, a);
    FP_TO_INT_S(r, A, SI_BITS, 0);
    FP_HANDLE_EXCEPTIONS;

    return r;
}