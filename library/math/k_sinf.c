/*
 * $Id: math_k_sinf.c,v 1.8 2023-07-19 12:04:24 clib2devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

/* |sin(x)/x - s(x)| < 2**-37.5 (~[-4.89e-12, 4.824e-12]). */
static const double
        S1 = -0x15555554cbac77.0p-55,   /* -0.166666666416265235595 */
        S2 = 0x111110896efbb2.0p-59,    /*  0.0083333293858894631756 */
        S3 = -0x1a00f9e2cae774.0p-65,   /* -0.000198393348360966317347 */
        S4 = 0x16cd878c3b46a7.0p-71;    /*  0.0000027183114939898219064 */

float
__kernel_sindf(double x) {
    double r, s, w, z;

    /* Try to optimize for parallel evaluation as in k_tanf.c. */
    z = x * x;
    w = z * z;
    r = S3 + z * S4;
    s = z * x;
    return (x + s * (S1 + z * S2)) + s * w * r;
}