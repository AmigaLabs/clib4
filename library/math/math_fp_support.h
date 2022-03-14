/*
 * $Id: math_fp_support.h,v 1.5 2006-01-08 12:04:23 obarthel Exp $
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

#ifndef _MATH_FP_SUPPORT_H
#define _MATH_FP_SUPPORT_H

#include <endian.h>

/* 'Portable' raw representations of three IEEE floating point formats. */
union ieee_long_double
{
	long double		value;
	unsigned long	raw[3];
};

union ieee_double
{
	double			value;
	unsigned long	raw[2];
};

union ieee_single
{
	float			value;
	unsigned long	raw[1];
};

union IEEEl2bits {
    long double	e;
    struct {
        unsigned int	sign	:1;
        unsigned int	exp	    :11;
        unsigned int	manh	:20;
        unsigned int	manl	:32;
    } bits;
    struct {
        unsigned int	manh	:20;
        unsigned int	manl	:32;
        unsigned int	expsign	:12;
    } xbits;
};

union IEEEf2bits {
    float	f;
    struct {
        unsigned int	sign	:1;
		unsigned int	exp	    :8;
		unsigned int	man	    :23;
    } bits;
};

struct Double {
    double	a;
    double	b;
};

#define	DBL_MANH_SIZE	20
#define	DBL_MANL_SIZE	32

union IEEEd2bits {
    double	d;
    struct {
        unsigned int	sign	:1;
		unsigned int	exp 	:11;
		unsigned int	manh	:20;
		unsigned int	manl	:32;
    } bits;
};

#define	mask_nbit_l(u)	((void)0)
#define	LDBL_IMPLICIT_NBIT
#define	LDBL_NBIT	0

#define	LDBL_MANH_SIZE	20
#define	LDBL_MANL_SIZE	32

#define	LDBL_TO_ARRAY32(u, a) do {			\
	(a)[0] = (uint32_t)(u).bits.manl;		\
	(a)[1] = (uint32_t)(u).bits.manh;		\
} while(0)

extern float __inff(void);
extern double __inf(void);

/****************************************************************************/

#endif /* _MATH_FP_SUPPORT_H */
