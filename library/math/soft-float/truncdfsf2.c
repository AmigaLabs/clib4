/*
 * $Id: math_soft-float_truncdfsf2.c,v 1.0 2023-07-18 07:54:24 clib2devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

SFtype
__truncdfsf2(DFtype a) {
    FP_DECL_EX;
    FP_DECL_D (A);
    FP_DECL_S (R);
    SFtype r;

    FP_INIT_ROUNDMODE;
    FP_UNPACK_SEMIRAW_D (A, a);
#if _FP_W_TYPE_SIZE < _FP_FRACBITS_D
    FP_TRUNC (S, D, 1, 2, R, A);
#else
    FP_TRUNC (S, D, 1, 1, R, A);
#endif
    FP_PACK_SEMIRAW_S (r, R);
    FP_HANDLE_EXCEPTIONS;

    return r;
}