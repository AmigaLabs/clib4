/*
 * $Id: math_hypotf.c,v 1.4 2022-03-12 12:04:23 apalmate Exp $
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

#define SQRT_FLT_MAX 1.84467429742e+19 /* 0x5f7fffff */

float hypotf(float x, float y) {
    float a, b, t1, t2, y1, y2, w;
    int32_t j, k, ha, hb;

    GET_FLOAT_WORD(ha, x);
    ha &= 0x7fffffff;
    GET_FLOAT_WORD(hb, y);
    hb &= 0x7fffffff;
    if (hb > ha) {
        a = y;
        b = x;
        j = ha;
        ha = hb;
        hb = j;
    }
    else {
        a = x;
        b = y;
    }
    a = fabsf(a);
    b = fabsf(b);
    if ((ha - hb) > 0xf000000) { return a + b; } /* x/y > 2**30 */
    k = 0;
    if (ha > 0x58800000) {    /* a>2**50 */
        if (ha >= 0x7f800000) {    /* Inf or NaN */
            /* Use original arg order iff result is NaN; quieten sNaNs. */
            w = fabsf(x + 0.0F) - fabsf(y + 0.0F);
            if (ha == 0x7f800000) w = a;
            if (hb == 0x7f800000) w = b;
            return w;
        }
        /* scale a and b by 2**-68 */
        ha -= 0x22000000;
        hb -= 0x22000000;
        k += 68;
        SET_FLOAT_WORD(a, ha);
        SET_FLOAT_WORD(b, hb);
    }
    if (hb < 0x26800000) {    /* b < 2**-50 */
        if (hb <= 0x007fffff) {    /* subnormal b or 0 */
            if (hb == 0) return a;
            SET_FLOAT_WORD(t1, 0x7e800000);    /* t1=2^126 */
            b *= t1;
            a *= t1;
            k -= 126;
        } else {        /* scale a and b by 2^68 */
            ha += 0x22000000;    /* a *= 2^68 */
            hb += 0x22000000;    /* b *= 2^68 */
            k -= 68;
            SET_FLOAT_WORD(a, ha);
            SET_FLOAT_WORD(b, hb);
        }
    }
    /* medium size a and b */
    w = a - b;
    if (w > b) {
        SET_FLOAT_WORD(t1, ha & 0xfffff000);
        t2 = a - t1;
        w = sqrtf(t1 * t1 - (b * (-b) - t2 * (a + t1)));
    } else {
        a = a + a;
        SET_FLOAT_WORD(y1, hb & 0xfffff000);
        y2 = b - y1;
        SET_FLOAT_WORD(t1, (ha + 0x00800000) & 0xfffff000);
        t2 = a - t1;
        w = sqrtf(t1 * y1 - (w * (-w) - (t1 * y2 + t2 * b)));
    }
    if (k != 0) {
        SET_FLOAT_WORD(t1, 0x3f800000 + (k << 23));
        return t1 * w;
    } else return w;
}
