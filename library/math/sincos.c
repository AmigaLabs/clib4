/*
 * $Id: math_sincos.c,v 1.0 2022-03-11 12:04:23 apalmate Exp $
 *
 * :ts=4
 *
 * Portable ISO 'C' (1994) runtime library for the Amiga computer
 * Copyright (c) 2002-2015 by Olaf Barthel <obarthel (at) gmx.net>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   - Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *
 *   - Neither the name of Olaf Barthel nor the names of contributors
 *     may be used to endorse or promote products derived from this
 *     software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

/* Constants used in polynomial approximation of sin/cos */
static const double
        one = 1.00000000000000000000e+00,   /* 0x3FF00000, 0x00000000 */
        half = 5.00000000000000000000e-01,  /* 0x3FE00000, 0x00000000 */
        S1 = -1.66666666666666324348e-01,   /* 0xBFC55555, 0x55555549 */
        S2 = 8.33333333332248946124e-03,    /* 0x3F811111, 0x1110F8A6 */
        S3 = -1.98412698298579493134e-04,   /* 0xBF2A01A0, 0x19C161D5 */
        S4 = 2.75573137070700676789e-06,    /* 0x3EC71DE3, 0x57B1FE7D */
        S5 = -2.50507602534068634195e-08,   /* 0xBE5AE5E6, 0x8A2B9CEB */
        S6 = 1.58969099521155010221e-10,    /* 0x3DE5D93A, 0x5ACFD57C */
        C1 = 4.16666666666666019037e-02,    /* 0x3FA55555, 0x5555554C */
        C2 = -1.38888888888741095749e-03,   /* 0xBF56C16C, 0x16C15177 */
        C3 = 2.48015872894767294178e-05,    /* 0x3EFA01A0, 0x19CB1590 */
        C4 = -2.75573143513906633035e-07,   /* 0xBE927E4F, 0x809C52AD */
        C5 = 2.08757232129817482790e-09,    /* 0x3E21EE9E, 0xBDB4B1C4 */
        C6 = -1.13596475577881948265e-11;   /* 0xBDA8FAE9, 0xBE8838D4 */

static void
__kernel_sincos(double x, double y, int iy, double *k_s, double *k_c) {
    /* Inline calculation of sin/cos, as we can save
    some work, and we will always need to calculate
    both values, no matter the result of switch */
    double z, w, r, v, hz;
    z = x * x;
    w = z * z;

    /* cos-specific computation; equivalent to calling
     __kernel_cos(x,y) and storing in k_c*/
    r = z * (C1 + z * (C2 + z * C3)) + w * w * (C4 + z * (C5 + z * C6));
    hz = 0.5 * z;
    v = one - hz;

    *k_c = v + (((one - v) - hz) + (z * r - x * y));

    /* sin-specific computation; equivalent to calling
    __kernel_sin(x,y,1) and storing in k_s*/
    r = S2 + z * (S3 + z * S4) + z * w * (S5 + z * S6);
    v = z * x;
    if (iy == 0)
        *k_s = x + v * (S1 + z * r);
    else
        *k_s = x - ((z * (half * y - v * r) - y) - v * S1);
}

void
sincos(double x, double *s, double *c) {
    double y[2];
    int32_t ix;

    /* Store high word of x in ix */
    GET_HIGH_WORD(ix, x);

    /* |x| ~< pi/4 */
    ix &= 0x7fffffff;
    if (ix <= 0x3fe921fb) {
        /* Check for small x for sin and cos */
        if (ix < 0x3e46a09e) {
            /* Check for exact zero */
            if ((int) x == 0) {
                *s = x;
                *c = 1.0;
                return;
            }
        }
        /* Call kernel function with 0 extra */
        __kernel_sincos(x, 0.0, 0, s, c);
    } else if (ix >= 0x7ff00000) {
        /* sincos(Inf or NaN) is NaN */
        *s = x - x;
        *c = x - x;
    }

        /*argument reduction needed*/
    else {
        double k_c, k_s;

        /* Calculate remainer, then sub out to kernel */
        int32_t n = __rem_pio2(x, y);
        __kernel_sincos(y[0], y[1], 1, &k_s, &k_c);

        /* Figure out permutation of sin/cos outputs to true outputs */
        switch (n & 3) {
            case 0:
                *c = k_c;
                *s = k_s;
                break;
            case 1:
                *c = -k_s;
                *s = k_c;
                break;
            case 2:
                *c = -k_c;
                *s = -k_s;
                break;
            default:
                *c = k_s;
                *s = -k_c;
                break;
        }
    }
}
