/*
 * $Id: math_cbrtf.c,v 1.4 2022-03-13 12:04:23 apalmate Exp $
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

static const unsigned
        B1 = 709958130, /* B1 = (127-127.0/3-0.03306235651)*2**23 */
        B2 = 642849266; /* B2 = (127-127.0/3-24/3-0.03306235651)*2**23 */

float cbrtf(float x) {
    double r, T;
    float t;
    int32_t hx;
    uint32_t sign;
    uint32_t high;

    GET_FLOAT_WORD(hx, x);
    sign = hx & 0x80000000;        /* sign= sign(x) */
    hx ^= sign;
    if (hx >= 0x7f800000) return (x + x); /* cbrt(NaN,INF) is itself */

    /* rough cbrt to 5 bits */
    if (hx < 0x00800000) {        /* zero or subnormal? */
        if (hx == 0)
            return (x);        /* cbrt(+-0) is itself */
        SET_FLOAT_WORD(t, 0x4b800000); /* set t= 2**24 */
        t *= x;
        GET_FLOAT_WORD(high, t);
        SET_FLOAT_WORD(t, sign | ((high & 0x7fffffff) / 3 + B2));
    } else
        SET_FLOAT_WORD(t, sign | (hx / 3 + B1));

    /*
     * First step Newton iteration (solving t*t-x/t == 0) to 16 bits.  In
     * double precision so that its terms can be arranged for efficiency
     * without causing overflow or underflow.
     */
    T = t;
    r = T * T * T;
    T = T * ((double) x + x + r) / (x + r + r);

    /*
     * Second step Newton iteration to 47 bits.  In double precision for
     * efficiency and accuracy.
     */
    r = T * T * T;
    T = T * ((double) x + x + r) / (x + r + r);

    /* rounding to 24 bits is perfect in round-to-nearest mode */
    return (T);
}
