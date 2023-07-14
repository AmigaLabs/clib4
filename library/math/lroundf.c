/*
 * $Id: math_lroundf.c,v 1.5 2022-03-13 12:04:23 clib2devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

long
lroundf(float x) {
    int32_t j0;
    uint32_t i;
    long int result;
    int sign;

    GET_FLOAT_WORD (i, x);
    j0 = ((i >> 23) & 0xff) - 0x7f;
    sign = (i & 0x80000000) != 0 ? -1 : 1;
    i &= 0x7fffff;
    i |= 0x800000;

    if (j0 < (int32_t)(8 * sizeof(long int)) - 1) {
        if (j0 < 0)
            return j0 < -1 ? 0 : sign;
        else if (j0 >= 23)
            result = (long int) i << (j0 - 23);
        else {
            i += 0x400000 >> j0;

            result = i >> (23 - j0);
        }
    } else {
        /* The number is too large.  It is left implementation defined what happens.  */
        feraiseexcept(FE_INVALID);
        return (long int) x;
    }

    return sign * result;
}
