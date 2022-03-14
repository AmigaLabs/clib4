/*
 * $Id: math_asinl.c,v 1.0 2022-03-10 12:04:23 apalmate Exp $
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

#include "invtrig.h"

static const long double
        one =  1.00000000000000000000e+00,
        huge = 1.000e+300;

INLINE STATIC long double
__asinl(long double x) {
    union IEEEl2bits u;
    long double t = 0.0, w, p, q, c, r, s;
    int16_t expsign, expt;
    u.e = x;
    expsign = u.xbits.expsign;
    expt = expsign & 0x7fff;
    if (expt >= BIAS) {        /* |x|>= 1 */
        if (expt == BIAS && ((u.bits.manh & ~LDBL_NBIT) | u.bits.manl) == 0)
            /* asin(1)=+-pi/2 with inexact */
            return x * pio2_hi + x * pio2_lo;
        return (x - x) / (x - x);        /* asin(|x|>1) is NaN */
    } else if (expt < BIAS - 1) {    /* |x|<0.5 */
        if (expt < ASIN_LINEAR) {    /* if |x| is small, asinl(x)=x */
            if (huge + x > one) return x;/* return x with inexact if x!=0*/
        }
        t = x * x;
        p = P(t);
        q = Q(t);
        w = p / q;
        return x + x * w;
    }
    /* 1> |x|>= 0.5 */
    w = one - fabsl(x);
    t = w * 0.5;
    p = P(t);
    q = Q(t);
    s = sqrtl(t);
    if (u.bits.manh >= THRESH) {    /* if |x| is close to 1 */
        w = p / q;
        t = pio2_hi - (2.0 * (s + s * w) - pio2_lo);
    } else {
        u.e = s;
        u.bits.manl = 0;
        w = u.e;
        c = (t - w * w) / (s + w);
        r = p / q;
        p = 2.0 * s * r - (pio2_lo - 2.0 * c);
        q = pio4_hi - 2.0 * w;
        t = pio4_hi - (p - q);
    }
    if (expsign > 0) return t; else return -t;
}

long double
asinl(long double x) {
    long double result;

    if (-1.0 <= x && x <= 1.0) {
        result = __asinl(x);
    } else {
        result = NAN;
        __set_errno(EDOM);
    }

    return (result);
}