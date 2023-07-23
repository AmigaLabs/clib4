/*
 * $Id: math_soft-float_extendsfdf2.c,v 1.0 2023-07-18 07:54:24 clib2devs Exp $
*/

#define FP_NO_EXACT_UNDERFLOW
#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

DFtype
__extendsfdf2(SFtype a) {
    FP_DECL_EX;
    FP_DECL_S(A);
    FP_DECL_D(R);
    DFtype r;

    FP_INIT_EXCEPTIONS;
    FP_UNPACK_RAW_S(A, a);
#if _FP_W_TYPE_SIZE < _FP_FRACBITS_D
    FP_EXTEND (D, S, 2, 1, R, A);
#else
    FP_EXTEND(D, S, 1, 1, R, A);
#endif
    FP_PACK_RAW_D(r, R);
    FP_HANDLE_EXCEPTIONS;

    return r;
}