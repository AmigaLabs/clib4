/*
 * $Id: math_s_llround.c,v 1.7 2023-07-19 12:04:23 clib2devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

long long
llround(double x) {
    int32_t j0;
    uint32_t i1, i0;
    long long int result;
    int sign;

    EXTRACT_WORDS (i0, i1, x);
    j0 = ((i0 >> 20) & 0x7ff) - 0x3ff;
    sign = (i0 & 0x80000000) != 0 ? -1 : 1;
    i0 &= 0xfffff;
    i0 |= 0x100000;

    if (j0 < 20) {
        if (j0 < 0)
            return j0 < -1 ? 0 : sign;
        else {
            i0 += 0x80000 >> j0;

            result = i0 >> (20 - j0);
        }
    } else if (j0 < (int32_t)(8 * sizeof(long long int)) - 1) {
        if (j0 >= 52)
            result = (((long long int) i0 << 32) | i1) << (j0 - 52);
        else {
            uint32_t j = i1 + (0x80000000 >> (j0 - 20));
            if (j < i1)
                ++i0;

            if (j0 == 20)
                result = (long long int) i0;
            else
                result = ((long long int) i0 << (j0 - 20)) | (j >> (52 - j0));
        }
    } else {
        /* The number is too large.  It is left implementation defined what happens.  */
        feraiseexcept(FE_INVALID);
        return (long long int) x;
    }

    return sign * result;
}
