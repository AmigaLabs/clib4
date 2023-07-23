/*
 * $Id: math_soft-float_eqsf2.c,v 1.0 2023-07-18 07:54:24 clib2devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

CMPtype
__eqsf2(SFtype a, SFtype b) {
    FP_DECL_EX;
    FP_DECL_S(A);
    FP_DECL_S(B);
    CMPtype r;

    FP_INIT_EXCEPTIONS;
    FP_UNPACK_RAW_S(A, a);
    FP_UNPACK_RAW_S(B, b);
    FP_CMP_EQ_S(r, A, B, 1);
    FP_HANDLE_EXCEPTIONS;

    return r;
}

CMPtype
__nesf2(SFtype a, SFtype b) {
    return __eqsf2(a, b);
}