/*
 * $Id: math_soft-float_divdf3.c,v 1.0 2023-07-18 07:54:24 clib2devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

DFtype
__divdf3(DFtype a, DFtype b) {
    FP_DECL_EX;
    FP_DECL_D(A);
    FP_DECL_D(B);
    FP_DECL_D(R);
    DFtype r;

    FP_INIT_ROUNDMODE;
    FP_UNPACK_D(A, a);
    FP_UNPACK_D(B, b);
    FP_DIV_D(R, A, B);
    FP_PACK_D(r, R);
    FP_HANDLE_EXCEPTIONS;

    return r;
}