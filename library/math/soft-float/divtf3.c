/*
 * $Id: math_soft-float_divtf3.c,v 1.0 2023-07-18 07:54:24 clib2devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

TFtype
__divtf3(TFtype a, TFtype b) {
    FP_DECL_EX;
    FP_DECL_Q(A);
    FP_DECL_Q(B);
    FP_DECL_Q(R);
    TFtype r;

    FP_INIT_ROUNDMODE;
    FP_UNPACK_Q(A, a);
    FP_UNPACK_Q(B, b);
    FP_DIV_Q(R, A, B);
    FP_PACK_Q(r, R);
    FP_HANDLE_EXCEPTIONS;

    return r;
}