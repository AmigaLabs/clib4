/*
 * $Id: math_llroundl.c,v 1.1 2023-07-14 12:04:23 clib2devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

long long
llroundl(long double x) {
    int32_t j0;
    uint32_t se, i1, i0;
    long long int result;
    int sign;

    GET_LDOUBLE_WORDS (se, i0, i1, x);
    j0 = (se & 0x7fff) - 0x3fff;
    sign = (se & 0x8000) != 0 ? -1 : 1;

    if (j0 < 31) {
        if (j0 < 0)
            return j0 < -1 ? 0 : sign;
        else {
            uint32_t j = i0 + (0x40000000 >> j0);
            if (j < i0) {
                j >>= 1;
                j |= 0x80000000;
                ++j0;
            }

            result = j >> (31 - j0);
        }
    } else if (j0 < (int32_t)(8 * sizeof(long long int)) - 1) {
        if (j0 >= 63)
            result = (((long long int) i0 << 32) | i1) << (j0 - 63);
        else {
            uint32_t j = i1 + (0x80000000 >> (j0 - 31));

            result = (long long int) i0;
            if (j < i1)
                ++result;

            if (j0 > 31)
                result = (result << (j0 - 31)) | (j >> (63 - j0));
        }
    } else {
        /* The number is too large.  It is left implementation defined what happens.  */
        feraiseexcept(FE_INVALID);
        return (long long int) x;
    }

    return sign * result;
}
