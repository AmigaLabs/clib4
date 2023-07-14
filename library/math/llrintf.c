/*
 * $Id: math_llrintf.c,v 1.0 2022-03-11 12:04:23 clib2devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

static const float two23[2] = {
    8.3886080000e+06,   /* 0x4B000000 */
    -8.3886080000e+06,  /* 0xCB000000 */
};

long long
llrintf(float x) {
    fenv_t env;
    long long d;

    feholdexcept(&env);

    int32_t j0;
    uint32_t i0;
    volatile float w;
    float t;
    long long int result;
    int sx;

    GET_FLOAT_WORD (i0, x);

    sx = i0 >> 31;
    j0 = ((i0 >> 23) & 0xff) - 0x7f;
    i0 &= 0x7fffff;
    i0 |= 0x800000;

    if (j0 < (int32_t)(sizeof(long long int) * 8) - 1) {
        if (j0 >= 23)
            result = (long long int) i0 << (j0 - 23);
        else {
            w = two23[sx] + x;
            t = w - two23[sx];
            GET_FLOAT_WORD (i0, t);
            j0 = ((i0 >> 23) & 0xff) - 0x7f;
            i0 &= 0x7fffff;
            i0 |= 0x800000;

            result = (j0 < 0 ? 0 : i0 >> (23 - j0));
        }
    } else {
        if (fetestexcept(FE_INVALID))
            feclearexcept(FE_INEXACT);
        feupdateenv(&env);

        /* The number is too large. It is left implementation defined what happens.  */
        return (long long int) x;
    }

    if (fetestexcept(FE_INVALID))
        feclearexcept(FE_INEXACT);
    feupdateenv(&env);

    return sx ? -result : result;
}
