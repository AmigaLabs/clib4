/*
 * $Id: math_soft-float_lesf2.c,v 1.0 2023-07-18 07:54:24 clib2devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

CMPtype
__lesf2(SFtype a, SFtype b) {
    FP_DECL_EX;
    FP_DECL_S(A);
    FP_DECL_S(B);
    CMPtype r;

    FP_INIT_EXCEPTIONS;
    FP_UNPACK_RAW_S(A, a);
    FP_UNPACK_RAW_S(B, b);
    FP_CMP_S(r, A, B, 2, 2);
    FP_HANDLE_EXCEPTIONS;

    return r;
}
CMPtype
__ltsf2(SFtype a, SFtype b) {
    return __lesf2(a, b);
}