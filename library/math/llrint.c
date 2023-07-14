/*
 * $Id: math_llrint.c,v 1.2 2022-03-13 10:09:48 clib2devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

static const double two52[2] = {
    4.50359962737049600000e+15,  /* 0x43300000, 0x00000000 */
    -4.50359962737049600000e+15, /* 0xC3300000, 0x00000000 */
};

long long
llrint(double x) {
    fenv_t env;
    feholdexcept(&env);

    int32_t j0;
    uint32_t i1, i0;
    long long int result;
    volatile double w;
    double t;
    int sx;

    EXTRACT_WORDS (i0, i1, x);
    j0 = ((i0 >> 20) & 0x7ff) - 0x3ff;
    sx = i0 >> 31;
    i0 &= 0xfffff;
    i0 |= 0x100000;

    if (j0 < 20) {
        w = two52[sx] + x;
        t = w - two52[sx];
        EXTRACT_WORDS (i0, i1, t);
        j0 = ((i0 >> 20) & 0x7ff) - 0x3ff;
        i0 &= 0xfffff;
        i0 |= 0x100000;

        result = (j0 < 0 ? 0 : i0 >> (20 - j0));
    } else if (j0 < (int32_t)(8 * sizeof(long long int)) - 1) {
        if (j0 >= 52)
            result = (((long long int) i0 << 32) | i1) << (j0 - 52);
        else {
            w = two52[sx] + x;
            t = w - two52[sx];
            EXTRACT_WORDS (i0, i1, t);
            j0 = ((i0 >> 20) & 0x7ff) - 0x3ff;
            i0 &= 0xfffff;
            i0 |= 0x100000;

            if (j0 == 20)
                result = (long long int) i0;
            else
                result = ((long long int) i0 << (j0 - 20)) | (i1 >> (52 - j0));
        }
    } else {
        /* The number is too large.  It is left implementation defined
       what happens.  */
        return (long long int) x;
    }

    if (fetestexcept(FE_INVALID))
        feclearexcept(FE_INEXACT);
    feupdateenv(&env);
    return sx ? -result : result;
}
