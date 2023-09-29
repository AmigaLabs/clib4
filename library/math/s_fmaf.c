/*
 * $Id: math_s_fmaf.c,v 1.4 2023-07-19 12:04:23 clib4devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

float
fmaf(float x, float y, float z) {
    double xy, result;
    uint32_t hr, lr;

    xy = (double) x * y;
    result = xy + z;
    EXTRACT_WORDS(hr, lr, result);
    /* Common case: The double precision result is fine. */
    if ((lr & 0x1fffffff) != 0x10000000 ||    /* not a halfway case */
        (hr & 0x7ff00000) == 0x7ff00000 ||    /* NaN */
        result - xy == z ||            /* exact */
        fegetround() != FE_TONEAREST)    /* not round-to-nearest */
        return (result);

    /*
     * If result is inexact, and exactly halfway between two float values,
     * we need to adjust the low-order bit in the direction of the error.
     */
    fesetround(FE_TOWARDZERO);
    volatile double vxy = xy;  /* XXX work around gcc CSE bug */
    double adjusted_result = vxy + z;
    fesetround(FE_TONEAREST);
    if (result == adjusted_result)
        SET_LOW_WORD(adjusted_result, lr + 1);
    return (adjusted_result);
}
