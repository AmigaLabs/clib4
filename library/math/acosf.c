/*
 * $Id: math_acosf.c,v 1.3 2006-01-08 12:04:23 obarthel Exp $
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

static const float
        one = 1.0000000000e+00, /* 0x3F800000 */
        pi = 3.1415925026e+00, /* 0x40490fda */
        pio2_hi = 1.5707962513e+00; /* 0x3fc90fda */
static volatile float
        pio2_lo = 7.5497894159e-08; /* 0x33a22168 */
static const float
        pS0 = 1.6666586697e-01,
        pS1 = -4.2743422091e-02,
        pS2 = -8.6563630030e-03,
        qS1 = -7.0662963390e-01;

float
acosf(float x) {
    float z, p, q, r, w, s, c, df;
    int32_t hx, ix;
    GET_FLOAT_WORD(hx, x);
    ix = hx & 0x7fffffff;
    if (ix >= 0x3f800000) {        /* |x| >= 1 */
        if (ix == 0x3f800000) {    /* |x| == 1 */
            if (hx > 0) return 0.0;    /* acos(1) = 0 */
            else return pi + (float) 2.0 * pio2_lo;    /* acos(-1)= pi */
        }
        return (x - x) / (x - x);        /* acos(|x|>1) is NaN */
    }
    if (ix < 0x3f000000) {    /* |x| < 0.5 */
        if (ix <= 0x32800000) return pio2_hi + pio2_lo;/*if|x|<2**-26*/
        z = x * x;
        p = z * (pS0 + z * (pS1 + z * pS2));
        q = one + z * qS1;
        r = p / q;
        return pio2_hi - (x - (pio2_lo - x * r));
    } else if (hx < 0) {        /* x < -0.5 */
        z = (one + x) * (float) 0.5;
        p = z * (pS0 + z * (pS1 + z * pS2));
        q = one + z * qS1;
        s = sqrtf(z);
        r = p / q;
        w = r * s - pio2_lo;
        return pi - (float) 2.0 * (s + w);
    } else {            /* x > 0.5 */
        int32_t idf;
        z = (one - x) * (float) 0.5;
        s = sqrtf(z);
        df = s;
        GET_FLOAT_WORD(idf, df);
        SET_FLOAT_WORD(df, idf & 0xfffff000);
        c = (z - df * df) / (s + df);
        p = z * (pS0 + z * (pS1 + z * pS2));
        q = one + z * qS1;
        r = p / q;
        w = r * s + c;
        return (float) 2.0 * (df + w);
    }
}