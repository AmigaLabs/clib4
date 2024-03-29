/*
 * $Id: math_k_cosf.c,v 1.8 2023-07-19 12:04:24 clib4devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

/* |cos(x) - c(x)| < 2**-34.1 (~[-5.37e-11, 5.295e-11]). */
static const double
        one = 1.0,
        C0 = -0x1ffffffd0c5e81.0p-54,    /* -0.499999997251031003120 */
        C1 = 0x155553e1053a42.0p-57,    /*  0.0416666233237390631894 */
        C2 = -0x16c087e80f1e27.0p-62,    /* -0.00138867637746099294692 */
        C3 = 0x199342e0ee5069.0p-68;    /*  0.0000243904487962774090654 */

float
__kernel_cosdf(double x) {
    double r, w, z;

    /* Try to optimize for parallel evaluation as in k_tanf.c. */
    z = x * x;
    w = z * z;
    r = C2 + z * C3;
    return ((one + z * C0) + w * C1) + (w * z) * r;
}
