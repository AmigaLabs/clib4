/*
 * $Id: math.h,v 1.22 2007-01-06 10:09:49 clib4devs Exp $
*/

#ifndef _MATH_H
#define _MATH_H

#include <features.h>

__BEGIN_DECLS

#ifndef _STDLIB_H
#include <stdlib.h>
#endif /* _STDLIB_H */

/* Natural log of 2 */
#define _M_LN2        (double) 0.693147180559945309417

extern double acos(double x);
extern double asin(double x);
extern double atan(double x);
extern double atan2(double y, double x);
extern double ceil(double x);
extern double cos(double x);
extern double cosh(double x);
extern void sincos(double x, double *s, double *c);
extern void sincosf(float x, float *s, float *c);
extern void sincosl(double x, double *s, double *c);
extern double exp10(double x);
extern float exp10f(float x);
extern long double exp10l(long double x);
extern double exp(double x);
extern double fabs(double x);
extern double floor(double x);
extern double fmod(double x, double y);
extern double frexp(double x, int *nptr);
extern double ldexp(double x, int n);
extern double log(double x);
extern double log10(double x);
extern double modf(double x, double *nptr);
extern double pow(double x, double y);
extern double pow10(double x);
extern float pow10f(float x);
extern long double pow10l(long double x);
extern double sin(double x);
extern double sinh(double x);
extern double sqrt(double x);
extern double tan(double x);
extern double tanh(double x);
extern double j0(double x);
extern double y0(double x);
extern float j0f(float x);
extern float y0f(float x);
extern double j1(double x);
extern double y1(double x);
extern float j1f(float x);
extern float y1f(float x);
extern double jn(int n, double x);
extern double yn(int n, double x);
extern float jnf(int n, float x);
extern float ynf(int n, float x);

#define    M_E          (double) 2.7182818284590452354    /* e */
#define    M_LOG2E      (double) 1.4426950408889634074    /* log 2e */
#define    M_LOG10E     (double) 0.43429448190325182765    /* log 10e */
#define    M_LN2        (double) 0.69314718055994530942    /* log e2 */
#define    M_LN10       (double) 2.30258509299404568402    /* log e10 */
#define    M_PI         (double) 3.14159265358979323846    /* pi */
#define    M_PI_2       (double) 1.57079632679489661923    /* pi/2 */
#define    M_PI_4       (double) 0.78539816339744830962    /* pi/4 */
#define    M_1_PI       (double) 0.31830988618379067154    /* 1/pi */
#define    M_2_PI       (double) 0.63661977236758134308    /* 2/pi */
#define    M_2_SQRTPI   (double) 1.12837916709551257390    /* 2/sqrt(pi) */
#define    M_SQRT2      (double) 1.41421356237309504880    /* sqrt(2) */
#define    M_SQRT1_2    (double) 0.70710678118654752440    /* 1/sqrt(2) */

#define    MAXFLOAT    ((float) 3.40282346638528860e+38)

#define    FP_ILOGB0    (-INT_MAX)
#define    FP_ILOGBNAN    INT_MAX

/* HUGE_VALF is a 'float' Infinity.  */
#define HUGE_VALF (1.0f / 0.0f)
/* HUGE_VAL is a 'double' Infinity.  */
#define HUGE_VAL (1.0 / 0.0)
/* HUGE_VALL is a 'long double' Infinity.  */
#define HUGE_VALL (1.0L / 0.0L)

# ifndef INFINITY
#  define INFINITY (__builtin_inff())
# endif

# ifndef NAN
#  define NAN (__builtin_nanf(""))
# endif

#define FP_INFINITE     0x01   /* -/+ infinity */
#define FP_NAN          0x02   /* not a number */
#define FP_NORMAL       0x04   /* normalized floating point number */
#define FP_SUBNORMAL    0x08   /* very small floating point number; special case of IEEE 754 */
#define FP_ZERO         0x10   /* exponent/fraction are zero */

extern int __fpclassify_float(float x);
extern int __fpclassify_double(double x);
extern int __fpclassify_long_double(long double x);

extern int __isfinite_float(float x);
extern int __isfinite_double(double x);
extern int __isfinite_long_double(long double x);

extern int __signbit_float(float x);
extern int __signbit_double(double x);
extern int __signbit_long_double(long double x);

extern int __isnan(double x);
extern int __isnanf(float x);
extern int __isnanl(long double x);

extern int __isinf(double x);
extern int __isinff(float x);
extern int __isinfl(long double x);

extern int __isnormalf(float x);
extern int __isnormal(double x);
extern int __isnormall(long double x);

/* The gamma functions use a global variable, signgam.  */
#define signgam (*__signgam())
extern int *__signgam(void);

#define	fpclassify(x) \
    ((sizeof (x) == sizeof (float)) ? __fpclassify_float(x) \
    : (sizeof (x) == sizeof (double)) ? __fpclassify_double(x) \
    : __fpclassify_long_double(x))

#define isfinite(x) \
	(sizeof(x) == sizeof(float) ? __isfinite_float(x) \
    : (sizeof (x) == sizeof (double)) ? __isfinite_double(x) \
    : __isfinite_long_double(x))

#define	isnormal(x)					\
    ((sizeof (x) == sizeof (float)) ? __isnormalf(x)	\
    : (sizeof (x) == sizeof (double)) ? __isnormal(x)	\
    : __isnormall(x))

#define	signbit(x)					\
    ((sizeof (x) == sizeof (float)) ? __signbit_float(x)	\
    : (sizeof (x) == sizeof (double)) ? __signbit_double(x)	\
    : __signbit_long_double(x))

#define	isinf(x)					\
    ((sizeof (x) == sizeof (float)) ? __isinff(x)	\
    : (sizeof (x) == sizeof (double)) ? __isinf(x)	\
    : __isinfl(x))

#define isnan(x) \
	((sizeof(x) == sizeof(float)) ? __isnanf(x) \
    : (sizeof (x) == sizeof (double)) ? __isnan(x) \
    :  __isnanl(x))

#define    isgreater(x, y)         (!isunordered((x), (y)) && (x) > (y))
#define    isgreaterequal(x, y)    (!isunordered((x), (y)) && (x) >= (y))
#define    isless(x, y)            (!isunordered((x), (y)) && (x) < (y))
#define    islessequal(x, y)       (!isunordered((x), (y)) && (x) <= (y))
#define    islessgreater(x, y)     (!isunordered((x), (y)) && ((x) > (y) || (y) > (x)))
#define    isunordered(x, y)       (isnan(x) || isnan(y))
#if defined(__cplusplus)
#define    isinf(x) __isinf(x)
#define    isnan(x) __isnan(x)
#endif

#define finitef(x) __isfinite_float(x)
#define finitel(x) __isfinite_long_double(x)
#define isinff(x)  __isinff(x)
#define isnanf(x)  __isnanf(x)

extern float acosf(float x);
extern float asinf(float x);
extern float atanf(float x);
extern float atan2f(float y, float x);
extern float ceilf(float x);
extern float cosf(float x);
extern float coshf(float x);
extern float expf(float x);
extern float fabsf(float x);
extern float floorf(float x);
extern float fmodf(float x, float y);
extern float frexpf(float x, int *eptr);
extern float ldexpf(float x, int exp);
extern float logf(float x);
extern float log10f(float x);
extern float modff(float x, float *iptr);
extern float powf(float x, float y);
extern float sinf(float x);
extern float sinhf(float x);
extern float sqrtf(float x);
extern float tanf(float x);
extern float tanhf(float x);


extern float acoshf(float x);
extern float asinhf(float x);
extern float atanhf(float x);
extern float cbrtf(float x);
extern float copysignf(float x, float y);
extern float erfcf(float x);
extern float erff(float x);
extern float expm1f(float x);
extern float fdimf(float x, float y);
extern float fmaf(float x, float y, float z);
extern float fmaxf(float x, float y);
extern float fminf(float x, float y);
extern float hypotf(float x, float y);
extern float lgammaf(float x);
extern float log1pf(float x);
extern float logbf(float x);
extern long long llrintf(float x);
extern long lrintf(float x);
extern long lroundf(float x);
extern long long llround(double x);
extern long long llroundf(float x);
extern float nanf(const char *tagp);
extern float nearbyintf(float x);
extern float nextafterf(float x, float y);
extern float nexttowardf(float x, long double y);
extern float remainderf(float x, float p);
extern float remquof(float x, float y, int *quo);
extern float rintf(float x);
extern float roundf(float x);
extern float scalblnf(float x, long n);
extern float scalbnf(float x, int n);
extern float tgammaf(float x);
extern float truncf(float x);
extern int ilogbf(float x);
extern int finite(double x);

extern double acosh(double x);
extern double asinh(double x);
extern double atanh(double x);
extern double cbrt(double x);
extern double copysign(double x, double y);
extern double erf(double x);
extern double erfc(double x);
extern double expm1(double x);
extern double fdim(double x, double y);
extern double fma(double x, double y, double z);
extern double fmax(double x, double y);
extern double fmin(double x, double y);
extern double hypot(double x, double y);
extern double lgamma(double x);
extern double log1p(double x);
extern double logb(double x);
extern long long llrint(double x);
extern long int lrint(double x);
extern long int lround(double x);
extern double nan(const char *tagp);
extern double nearbyint(double x);
extern double nexttoward(double x, long double y);
extern double nextafter(double x, double y);
extern double remainder(double x, double p);
extern double remquo(double x, double y, int *quo);
extern double rint(double x);
extern double round(double x);
extern double scalbln(double x, long n);
extern double scalbn(double x, int n);
extern double tgamma(double x);
extern double trunc(double x);
extern int ilogb(double x);
extern float exp2f(float x);
extern double exp2(double x);
extern long double exp2l(long double x);

#if !defined(__cplusplus) && !defined(shared_comp)
#define log2(x) (log (x) / _M_LN2)
#else
extern double log2(double x);
#endif

extern float log2f(float x);
extern long double log2l(long double x);
extern long double acosl(long double x);
extern long double asinl(long double x);
extern long double atanl(long double x);
extern long double atan2l(long double y, long double x);
extern long double ceill(long double x);
extern long double cosl(long double x);
extern long double coshl(long double x);
extern long double expl(long double x);
extern long double fabsl(long double x);
extern long double floorl(long double x);
extern long double fmodl(long double x, long double y);
extern long double frexpl(long double x, int *nptr);
extern long double ldexpl(long double x, int n);
extern long double logl(long double x);
extern long double log10l(long double x);
extern long double modfl(long double x, long double *iptr);
extern long double powl(long double x, long double y);
extern long double sinl(long double x);
extern long double sinhl(long double x);
extern long double sqrtl(long double x);
extern long double tanl(long double x);
extern long double tanhl(long double x);
extern long double acoshl(long double x);
extern long double asinhl(long double x);
extern long double atanhl(long double x);
extern long double cbrtl(long double x);
extern long double copysignl(long double x, long double y);
extern long double erfcl(long double x);
extern long double erfl(long double x);
extern long double expm1l(long double x);
extern long double fdiml(long double x, long double y);
extern long double fmal(long double x, long double y, long double z);
extern long double fmaxl(long double x, long double y);
extern long double fminl(long double x, long double y);
extern long double hypotl(long double x, long double y);
extern long double lgammal(long double x);
extern long double log1pl(long double x);
extern long double logbl(long double x);
extern long long int llrintl(long double x);
extern long int lrintl(long double x);
extern long long int llroundl(long double x);
extern long int lroundl(long double x);
extern long double nanl(const char *tagp);
extern long double nearbyintl(long double x);
extern long double nextafterl(long double x, long double y);
extern long double nexttowardl(long double x, long double y);
extern long double remainderl(long double x, long double p);
extern long double remquol(long double x, long double y, int *quo);
extern long double rintl(long double x);
extern long double roundl(long double x);
extern long double scalblnl(long double x, long int n);
extern long double scalbnl(long double x, int n);
extern long double tgammal(long double x);
extern long double truncl(long double x);
extern int ilogbl(long double x);

#define FLT_EVAL_METHOD 0

typedef float float_t;
typedef double double_t;

__END_DECLS

#endif /* _MATH_H */
