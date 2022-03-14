/*
 * $Id: math_tanf.c,v 1.3 2006-01-08 12:04:24 obarthel Exp $
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
        T[] =  {
        0x15554d3418c99f.0p-54,	/* 0.333331395030791399758 */
        0x1112fd38999f72.0p-55,	/* 0.133392002712976742718 */
        0x1b54c91d865afe.0p-57,	/* 0.0533812378445670393523 */
        0x191df3908c33ce.0p-58,	/* 0.0245283181166547278873 */
        0x185dadfcecf44e.0p-61,	/* 0.00297435743359967304927 */
        0x1362b9bf971bcd.0p-59,	/* 0.00946564784943673166728 */
};

float
__kernel_tandf(double x, int iy)
{
    double z,r,w,s,t,u;

    z	=  x*x;
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
    r = T[4]+z*T[5];
    t = T[2]+z*T[3];
    w = z*z;
    s = z*x;
    u = T[0]+z*T[1];
    r = (x+s*u)+(s*w)*(t+w*r);
    if(iy==1) return r;
    else return -1.0/r;
}

static const double
        t1pio2 = 1 * M_PI_2,			/* 0x3FF921FB, 0x54442D18 */
        t2pio2 = 2 * M_PI_2,            /* 0x400921FB, 0x54442D18 */
        t3pio2 = 3 * M_PI_2,            /* 0x4012D97C, 0x7F3321D2 */
        t4pio2 = 4 * M_PI_2;            /* 0x401921FB, 0x54442D18 */

float tanf(float x) {
    double y;
    int32_t n, hx, ix;

    GET_FLOAT_WORD(hx, x);
    ix = hx & 0x7fffffff;

    if (ix <= 0x3f490fda) {        /* |x| ~<= pi/4 */
        if (ix < 0x39800000)        /* |x| < 2**-12 */
            if (((int) x) == 0) return x;    /* x with inexact if x != 0 */
        return __kernel_tandf(x, 1);
    }
    if (ix <= 0x407b53d1) {        /* |x| ~<= 5*pi/4 */
        if (ix <= 0x4016cbe3)        /* |x| ~<= 3pi/4 */
            return __kernel_tandf(x + (hx > 0 ? -t1pio2 : t1pio2), -1);
        else
            return __kernel_tandf(x + (hx > 0 ? -t2pio2 : t2pio2), 1);
    }
    if (ix <= 0x40e231d5) {        /* |x| ~<= 9*pi/4 */
        if (ix <= 0x40afeddf)        /* |x| ~<= 7*pi/4 */
            return __kernel_tandf(x + (hx > 0 ? -t3pio2 : t3pio2), -1);
        else
            return __kernel_tandf(x + (hx > 0 ? -t4pio2 : t4pio2), 1);
    }

        /* tan(Inf or NaN) is NaN */
    else if (ix >= 0x7f800000) return x - x;

        /* general argument reduction needed */
    else {
        n = __rem_pio2f(x, &y);
        /* integer parameter: 1 -- n even; -1 -- n odd */
        return __kernel_tandf(y, 1 - ((n & 1) << 1));
    }
}
