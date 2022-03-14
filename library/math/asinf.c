/*
 * $Id: math_asinf.c,v 1.3 2006-01-08 12:04:23 obarthel Exp $
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
 */

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

static const float
        one = 1.0000000000e+00, /* 0x3F800000 */
        huge = 1.000e+30,
        /* coefficient for R(x^2) */
        pS0 = 1.6666586697e-01,
        pS1 = -4.2743422091e-02,
        pS2 = -8.6563630030e-03,
        qS1 = -7.0662963390e-01;

static const double
        pio2 = 1.570796326794896558e+00;

float
asinf(float x) {
    double s;
    float t, w, p, q;
    int32_t hx, ix;
    GET_FLOAT_WORD(hx, x);
    ix = hx & 0x7fffffff;
    if (ix >= 0x3f800000) {        /* |x| >= 1 */
        if (ix == 0x3f800000)        /* |x| == 1 */
            return x * pio2;        /* asin(+-1) = +-pi/2 with inexact */
        return (x - x) / (x - x);        /* asin(|x|>1) is NaN */
    } else if (ix < 0x3f000000) {    /* |x|<0.5 */
        if (ix < 0x39800000) {        /* |x| < 2**-12 */
            if (huge + x > one) return x;/* return x with inexact if x!=0*/
        }
        t = x * x;
        p = t * (pS0 + t * (pS1 + t * pS2));
        q = one + t * qS1;
        w = p / q;
        return x + x * w;
    }
    /* 1> |x|>= 0.5 */
    w = one - fabsf(x);
    t = w * (float) 0.5;
    p = t * (pS0 + t * (pS1 + t * pS2));
    q = one + t * qS1;
    s = sqrt(t);
    w = p / q;
    t = pio2 - 2.0 * (s + s * w);
    if (hx > 0) return t; else return -t;
}