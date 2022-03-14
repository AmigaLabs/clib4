/*
 * $Id: math_sinf.c,v 1.3 2006-01-08 12:04:24 obarthel Exp $
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
 *
 *
 * PowerPC math library based in part on work by Sun Microsystems
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunPro, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice
 * is preserved.
 *
 *
 * Conversion to float by Ian Lance Taylor, Cygnus Support, ian@cygnus.com.
 */

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

static const double
        S1 = -0x15555554cbac77.0p-55,    /* -0.166666666416265235595 */
        S2 = 0x111110896efbb2.0p-59,    /*  0.0083333293858894631756 */
        S3 = -0x1a00f9e2cae774.0p-65,    /* -0.000198393348360966317347 */
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

static const double
        s1pio2 = 1*M_PI_2,			/* 0x3FF921FB, 0x54442D18 */
        s2pio2 = 2 * M_PI_2,            /* 0x400921FB, 0x54442D18 */
        s3pio2 = 3 * M_PI_2,            /* 0x4012D97C, 0x7F3321D2 */
        s4pio2 = 4 * M_PI_2;            /* 0x401921FB, 0x54442D18 */

float sinf(float x) {
    double y;
    int32_t n, hx, ix;

    GET_FLOAT_WORD(hx, x);
    ix = hx & 0x7fffffff;

    if (ix <= 0x3f490fda) {        /* |x| ~<= pi/4 */
        if (ix < 0x39800000)        /* |x| < 2**-12 */
            if (((int) x) == 0) return x;    /* x with inexact if x != 0 */
        return __kernel_sindf(x);
    }
    if (ix <= 0x407b53d1) {        /* |x| ~<= 5*pi/4 */
        if (ix <= 0x4016cbe3) {    /* |x| ~<= 3pi/4 */
            if (hx > 0)
                return __kernel_cosdf(x - s1pio2);
            else
                return -__kernel_cosdf(x + s1pio2);
        } else
            return __kernel_sindf((hx > 0 ? s2pio2 : -s2pio2) - x);
    }
    if (ix <= 0x40e231d5) {        /* |x| ~<= 9*pi/4 */
        if (ix <= 0x40afeddf) {    /* |x| ~<= 7*pi/4 */
            if (hx > 0)
                return -__kernel_cosdf(x - s3pio2);
            else
                return __kernel_cosdf(x + s3pio2);
        } else
            return __kernel_sindf(x + (hx > 0 ? -s4pio2 : s4pio2));
    }

        /* sin(Inf or NaN) is NaN */
    else if (ix >= 0x7f800000) return x - x;

        /* general argument reduction needed */
    else {
        n = __rem_pio2f(x, &y);
        switch (n & 3) {
            case 0:
                return __kernel_sindf(y);
            case 1:
                return __kernel_cosdf(y);
            case 2:
                return __kernel_sindf(-y);
            default:
                return -__kernel_cosdf(y);
        }
    }
}
