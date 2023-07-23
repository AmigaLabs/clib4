/*
 * $Id: math_soft-float_subdf3.c,v 1.0 2023-07-18 07:54:24 clib2devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

DFtype
__subdf3(DFtype a, DFtype b) {
    FP_DECL_EX;
    FP_DECL_D (A);
    FP_DECL_D (B);
    FP_DECL_D (R);
    DFtype r;

    FP_INIT_ROUNDMODE;
    FP_UNPACK_SEMIRAW_D (A, a);
    FP_UNPACK_SEMIRAW_D (B, b);
    FP_SUB_D (R, A, B);
    FP_PACK_SEMIRAW_D (r, R);
    FP_HANDLE_EXCEPTIONS;

    return r;
}