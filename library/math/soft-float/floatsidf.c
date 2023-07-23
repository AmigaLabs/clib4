/*
 * $Id: math_soft-float_floatsidf.c,v 1.0 2023-07-18 07:54:24 clib2devs Exp $
*/

#define FP_NO_EXCEPTIONS
#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

DFtype
__floatsidf(SItype i) {
    FP_DECL_D(A);
    DFtype a;

    FP_FROM_INT_D(A, i, SI_BITS, USItype);
    FP_PACK_RAW_D(a, A);

    return a;
}