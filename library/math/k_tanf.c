/*
 * $Id: math_k_tanf.c,v 1.0 2023-07-19 12:04:23 clib4devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

/* |tan(x)/x - t(x)| < 2**-25.5 (~[-2e-08, 2e-08]). */
static const double
    T[] = {
        0x15554d3418c99f.0p-54,    /* 0.333331395030791399758 */
        0x1112fd38999f72.0p-55,    /* 0.133392002712976742718 */
        0x1b54c91d865afe.0p-57,    /* 0.0533812378445670393523 */
        0x191df3908c33ce.0p-58,    /* 0.0245283181166547278873 */
        0x185dadfcecf44e.0p-61,    /* 0.00297435743359967304927 */
        0x1362b9bf971bcd.0p-59,    /* 0.00946564784943673166728 */
    };

float
__kernel_tandf(double x, int iy) {
    double z, r, w, s, t, u;

    z = x * x;
    /*
     * Split up the polynomial into small independent terms to give
     * opportunities for parallel evaluation.  The chosen splitting is
     * micro-optimized for Athlons (XP, X64).  It costs 2 multiplications
     * relative to Horner's method on sequential machines.
     *
     * We add the small terms from lowest degree up for efficiency on
     * non-sequential machines (the lowest degree terms tend to be ready
     * earlier).  Apart from this, we don't care about order of
     * operations, and don't need to to care since we have precision to
     * spare.  However, the chosen splitting is good for accuracy too,
     * and would give results as accurate as Horner's method if the
     * small terms were added from highest degree down.
     */
    r = T[4] + z * T[5];
    t = T[2] + z * T[3];
    w = z * z;
    s = z * x;
    u = T[0] + z * T[1];
    r = (x + s * u) + (s * w) * (t + w * r);
    if (iy == 1) return r;
    else return -1.0 / r;
}
