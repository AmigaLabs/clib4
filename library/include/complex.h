/*
 * $Id: complex.h,v 1.6 2006-01-08 12:06:14 obarthel Exp $
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
 *****************************************************************************
 *
 * Documentation and source code for this library, and the most recent library
 * build are available from <https://github.com/afxgroup/clib2>.
 *
 *****************************************************************************
 */

#ifndef _COMPLEX_H
#define _COMPLEX_H

/****************************************************************************/

/* The following is not part of the ISO 'C' (1994) standard, but it should
   be part of ISO/IEC 9899:1999, also known as "C99". */

/****************************************************************************/

#ifndef	__GNUC__
#error Unsupported compiler.
#endif /* __GNUC__ */

/****************************************************************************/

#if defined(__cplusplus)
#warning C99 header file used in C++.
#endif /* __cplusplus */

/****************************************************************************/

#if !defined(__STDC_VERSION__) || (__GNUC__ < 3 && __STDC_VERSION__ < 199901L)
#error Complex numbers are not supported by/for this compiler.
#endif /* __GNUC__ && __STDC_VERSION__ */

#include <features.h>

__BEGIN_DECLS

#define complex _Complex
#ifdef __GNUC__
#define _Complex_I (__extension__ 1.0fi)
#else
#define _Complex_I 1.0fi
#endif
#define I _Complex_I

/****************************************************************************/

extern double complex cacos(double complex z);
extern float complex cacosf(float complex z);
extern long double complex cacosl(long double complex z);

extern double complex casin(double complex z);
extern float complex casinf(float complex z);
extern long double complex casinl(long double complex z);

extern double complex catan(double complex z);
extern float complex catanf(float complex z);
extern long double complex catanl(long double complex z);

extern double complex ccos(double complex z);
extern float complex ccosf(float complex z);
extern long double complex ccosl(long double complex z);

extern double complex csin(double complex z);
extern float complex csinf(float complex z);
extern long double complex csinl(long double complex z);

extern double complex ctan(double complex z);
extern float complex ctanf(float complex z);
extern long double complex ctanl(long double complex z);

extern double complex cacosh(double complex z);
extern float complex cacoshf(float complex z);
extern long double complex cacoshl(long double complex z);

extern double complex casinh(double complex z);
extern float complex casinhf(float complex z);
extern long double complex casinhl(long double complex z);

extern double complex catanh(double complex z);
extern float complex catanhf(float complex z);
extern long double complex catanhl(long double complex z);

extern double complex ccosh(double complex z);
extern float complex ccoshf(float complex z);
extern long double complex ccoshl(long double complex z);

extern double complex csinh(double complex z);
extern float complex csinhf(float complex z);
extern long double complex csinhl(long double complex z);

extern double complex ctanh(double complex z);
extern float complex ctanhf(float complex z);
extern long double complex ctanhl(long double complex z);

extern double complex cexp(double complex z);
extern float complex cexpf(float complex z);
extern long double complex cexpl(long double complex z);

extern double complex clog(double complex z);
extern float complex clogf(float complex z);
extern long double complex clogl(long double complex z);

extern double cabs(double complex z);
extern float cabsf(float complex z);
extern long double cabsl(long double complex z);

extern double complex cpow(double complex z, double complex c);
extern float complex cpowf(float complex z, float complex c);
extern long double complex cpowl(long double complex z, long double complex c);

extern double complex csqrt(double complex z);
extern float complex csqrtf(float complex z);
extern long double complex csqrtl(long double complex z);

extern double complex carg(double complex z);
extern float complex cargf(float complex z);
extern long double complex cargl(long double complex z);

extern double complex conj(double complex z);
extern float complex conjf(float complex z);
extern long double complex conjl(long double complex z);

extern double complex cproj(double complex z);
extern float complex cprojf(float complex z);
extern long double complex cprojl(long double complex z);

/****************************************************************************/

double complex cimag(double complex z);
float complex cimagf(float complex z);
long double complex cimagl(long double complex z);


double complex creal(double complex z);
float complex crealf(float complex z);
long double complex creall(long double complex z);

/****************************************************************************/

#define __CIMAG(x, t) \
	((union { _Complex t __z; t __xy[2]; }){(_Complex t)(x)}.__xy[1])

#define creal(x) ((double)(x))
#define crealf(x) ((float)(x))
#define creall(x) ((long double)(x))

#define cimag(x) __CIMAG(x, double)
#define cimagf(x) __CIMAG(x, float)
#define cimagl(x) __CIMAG(x, long double)

#define __CMPLX(x, y, t) \
	((union { _Complex t __z; t __xy[2]; }){.__xy = {(x),(y)}}.__z)

#define CMPLX(x, y) __CMPLX(x, y, double)
#define CMPLXF(x, y) __CMPLX(x, y, float)
#define CMPLXL(x, y) __CMPLX(x, y, long double)

__END_DECLS

#endif /* _COMPLEX_H */
