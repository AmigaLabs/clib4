/*
 * $Id: complex.h,v 1.6 2006-01-08 12:06:14 clib4devs Exp $
*/

#ifndef _COMPLEX_H
#define _COMPLEX_H


#ifndef	__GNUC__
#error Unsupported compiler.
#endif /* __GNUC__ */

#if defined(__cplusplus)
#warning C99 header file used in C++.
#endif /* __cplusplus */

#if (!defined(__STDC_VERSION__) && __GCC_IEC_559_COMPLEX < 1) || \
    (__GNUC__ < 3 && __STDC_VERSION__ < 199901L)
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

extern double cimag(double complex z);
extern float cimagf(float complex z);
extern long double cimagl(long double complex z);
extern double creal(double complex z);
extern float crealf(float complex z);
extern long double creall(long double complex z);

#define __CMPLX(x, y, t) \
	((union { _Complex t __z; t __xy[2]; }){.__xy = {(x),(y)}}.__z)

#define CMPLX(x, y) __CMPLX(x, y, double)
#define CMPLXF(x, y) __CMPLX(x, y, float)
#define CMPLXL(x, y) __CMPLX(x, y, long double)

__END_DECLS

#endif /* _COMPLEX_H */
