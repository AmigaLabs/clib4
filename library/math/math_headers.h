/*
 * $Id: math_headers.h,v 1.15 2006-11-13 09:51:53 clib2devs Exp $
*/

#ifndef _MATH_HEADERS_H
#define _MATH_HEADERS_H

#include <limits.h>
#include <errno.h>
#include <float.h>
#include <math.h>
#include <sys/param.h>
#include <fenv.h>
#include <endian.h>

#ifndef _MACROS_H
#include "macros.h"
#endif /* _MACROS_H */

#ifndef _DEBUG_H
#include "debug.h"
#endif /* _DEBUG_H */

#ifndef _MATH_FP_SUPPORT_H
#include "math_fp_support.h"
#endif /* _MATH_FP_SUPPORT_H */

#ifndef _COMPLEX_HEADERS_H
#include "complex_headers.h"
#endif /* _COMPLEX_HEADERS_H */

typedef long double __float64;

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#include "gdtoa.h"
#include "mprec.h"

#define _FLOAT64_MIN  LDBL_MIN
#define __F_64(x)     x ## L
#define _F_64(x)      __F_64(x)


#define SAFE_LEFT_SHIFT(op,amt)					\
  (((amt) < 8 * sizeof(op)) ? ((op) << (amt)) : 0)

#define SAFE_RIGHT_SHIFT(op,amt)				\
  (((amt) < 8 * sizeof(op)) ? ((op) >> (amt)) : 0)

/****************************************************************************/
typedef const union
{
  long l[2];
  double d;
} udouble;

typedef const union
{
  long l;
  float f;
} ufloat;

STATIC ufloat z_hugeval_f  = { 0x7f800000 };
STATIC ufloat z_infinity_f = { 0x7f800000 };
STATIC ufloat z_notanum_f  = { 0xffd00000 };

STATIC udouble z_hugeval  = {{ 0x7ff00000, 0 }};
STATIC udouble z_infinity = {{ 0x7ff00000, 0 }};
STATIC udouble z_notanum  = {{ 0xfff80000, 0 }};
/*
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunPro, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice 
 * is preserved.
 * ====================================================
 */

#define FLT_UWORD_IS_ZERO(x) ((x)==0)
#define FLT_UWORD_IS_SUBNORMAL(x) ((x)<0x00800000L)
#define FLT_UWORD_MIN 0x00000001
#define FLT_UWORD_EXP_MIN 0x43160000
#define FLT_UWORD_LOG_MIN 0x42cff1b5
#define FLT_SMALLEST_EXP -22

#if __FLOAT_WORD_ORDER__ == __BIG_ENDIAN
#  define HIGH_HALF 0
#  define LOW_HALF  1
#else
#  if __FLOAT_WORD_ORDER__ == __LITTLE_ENDIAN
#error Cannot use LIITLE ENDIAN on AmigaOS4
#   define HIGH_HALF 1
#   define LOW_HALF  0
#  endif
#endif

#if __FLOAT_WORD_ORDER__ == __ORDER_BIG_ENDIAN__
typedef union
{
    double value;
    struct
    {
        uint32_t msw;
        uint32_t lsw;
    } parts;
    struct
    {
        uint64_t w;
    } xparts;
} ieee_double_shape_type;
#endif

#if __FLOAT_WORD_ORDER__ == __ORDER_LITTLE_ENDIAN__
#error Cannot use LIITLE ENDIAN on AmigaOS4
typedef union
{
    double value;
    struct
    {
        uint32_t lsw;
        uint32_t msw;
    } parts;
    struct
    {
        uint64_t w;
    } xparts;
} ieee_double_shape_type;
#endif

#if __FLOAT_WORD_ORDER__ == __ORDER_BIG_ENDIAN__
typedef union
{
    long double value;
    struct {
        uint32_t mswhi;
        uint32_t mswlo;
        uint32_t lswhi;
        uint32_t lswlo;
    } parts32;
    struct {
        uint64_t msw;
        uint64_t lsw;
    } parts64;
} ieee_quad_shape_type;
#endif

#if __FLOAT_WORD_ORDER__ == __ORDER_LITTLE_ENDIAN__
#error Cannot use LIITLE ENDIAN on AmigaOS4
typedef union
{
    long double value;
    struct {
        u_int32_t lswlo;
        u_int32_t lswhi;
        u_int32_t mswlo;
        u_int32_t mswhi;
    } parts32;
    struct {
        u_int64_t lsw;
        u_int64_t msw;
    } parts64;
} ieee_quad_shape_type;
#endif

/* A union which permits us to convert between a float and a 32 bit int. */
typedef union {
    float value;
    unsigned int word;
} ieee_float_shape_type;

typedef union {
    long double value;
    struct {
#ifdef __LP64__
        int padh:32;
#endif
        int exp:16;
        int padl:16;
        uint32_t msw;
        uint32_t lsw;
    } parts;
} ieee_extended_shape_type;

typedef struct {    /* This structure holds the details of a multi-precision     */
    int e;          /* floating point number, x: d[0] holds its sign (-1,0 or 1) */
    double d[40];   /* e holds its exponent (...,-2,-1,0,1,2,...) and            */
} mp_no;            /* d[1]...d[p] hold its mantissa digits. The value of x is,  */
/* x = d[1]*r**(e-1) + d[2]*r**(e-2) + ... + d[p]*r**(e-p).  */
/* Here   r = 2**24,   0 <= d[i] < r  and  1 <= p <= 32.     */
/* p is a global variable. A multi-precision number is       */
/* always normalized. Namely, d[1] > 0. An exception is      */
/* a zero which is characterized by d[0] = 0. The terms      */
/* d[p+1], d[p+2], ... of a none zero number have no         */
/* significance and so are the terms e, d[1],d[2],...        */
/* of a zero.                                                */

typedef int int4;
typedef union {int4 i[2]; double x;} mynumber;
typedef union { int i[2]; double d; } number;
typedef union { double d; uint32_t i[2]; } U;

#define ABS(x)   (((x)>0)?(x):-(x))
#define max(x,y)  (((y)>(x))?(y):(x))
#define min(x,y)  (((y)<(x))?(y):(x))

#define EXTRACT_WORDS(ix0, ix1, d)					\
do {												\
    ieee_double_shape_type ew_u;					\
    ew_u.value = (d);								\
    (ix0) = ew_u.parts.msw;							\
    (ix1) = ew_u.parts.lsw;							\
} while (0)

/* Get a 64-bit int from a double. */
#define EXTRACT_WORD64(ix,d)					    \
do {								                \
  ieee_double_shape_type ew_u;					    \
  ew_u.value = (d);						            \
  (ix) = ew_u.xparts.w;						        \
} while (0)

/* Get the more significant 32 bit int from a double.  */

#define GET_HIGH_WORD(i,d)							\
do {												\
    ieee_double_shape_type gh_u;					\
    gh_u.value = (d);								\
    (i) = gh_u.parts.msw;							\
} while (0)

/* Get the less significant 32 bit int from a double.  */

#define GET_LOW_WORD(i,d)							\
do {												\
    ieee_double_shape_type gl_u;					\
    gl_u.value = (d);								\
    (i) = gl_u.parts.lsw;							\
} while (0)

/* Set a double from two 32 bit ints.  */
#define INSERT_WORDS(d,ix0,ix1)						\
do {												\
  ieee_double_shape_type iw_u;						\
  iw_u.parts.msw = (ix0);							\
  iw_u.parts.lsw = (ix1);							\
  (d) = iw_u.value;									\
} while (0)

/* Set a double from a 64-bit int. */
#define INSERT_WORD64(d,ix)					        \
do {								                \
  ieee_double_shape_type iw_u;					    \
  iw_u.xparts.w = (ix);						        \
  (d) = iw_u.value;						            \
} while (0)

/* Set the more significant 32 bits of a double from an int.  */
#define SET_HIGH_WORD(d,v)							\
do {												\
  ieee_double_shape_type sh_u;						\
  sh_u.value = (d);									\
  sh_u.parts.msw = (v);								\
  (d) = sh_u.value;									\
} while (0)

/* Set the less significant 32 bits of a double from an int.  */

#define SET_LOW_WORD(d,v)							\
do {												\
  ieee_double_shape_type sl_u;						\
  sl_u.value = (d);									\
  sl_u.parts.lsw = (v);								\
  (d) = sl_u.value;									\
} while (0)

/* Get three 32 bit ints from a double.  */

#define GET_LDOUBLE_WORDS(se,ix0,ix1,d)				\
do {								                \
  ieee_extended_shape_type ew_u = {0};				\
  ew_u.value = (d);						            \
  (se) = ew_u.parts.exp;					        \
  (ix0) = ew_u.parts.msw;					        \
  (ix1) = ew_u.parts.lsw;					        \
} while (0)

/* Set a double from two 32 bit ints.  */

#define SET_LDOUBLE_WORDS(d,se,ix0,ix1)				\
do {								                \
  ieee_extended_shape_type iw_u;				    \
  iw_u.parts.exp = (se);					        \
  iw_u.parts.msw = (ix0);					        \
  iw_u.parts.lsw = (ix1);					        \
  (d) = iw_u.value;						            \
} while (0)

/* Get the more significant 32 bits of a long double mantissa.  */

#define GET_LDOUBLE_MSW(v,d)					    \
do {								                \
  ieee_extended_shape_type sh_u;				    \
  sh_u.value = (d);						            \
  (v) = sh_u.parts.msw;						        \
} while (0)

/* Set the more significant 32 bits of a long double mantissa from an int.  */

#define SET_LDOUBLE_MSW(d,v)					    \
do {								                \
  ieee_extended_shape_type sh_u;				    \
  sh_u.value = (d);						            \
  sh_u.parts.msw = (v);						        \
  (d) = sh_u.value;						            \
} while (0)

/* Get int from the exponent of a long double.  */

#define GET_LDOUBLE_EXP(se,d)					    \
do {								                \
  ieee_extended_shape_type ge_u;				    \
  ge_u.value = (d);						            \
  (se) = ge_u.parts.exp;					        \
} while (0)

/* Set exponent of a long double from an int.  */

#define SET_LDOUBLE_EXP(d,se)					    \
do {								                \
  ieee_extended_shape_type se_u;				    \
  se_u.value = (d);						            \
  se_u.parts.exp = (se);					        \
  (d) = se_u.value;						            \
} while (0)

/* Get two 64 bit ints from a long double.  */

#define GET_LDOUBLE_WORDS64(ix0, ix1, d)	        \
do {								                \
  ieee_quad_shape_type qw_u = { 0 };		        \
  qw_u.value = (d);						            \
  (ix0) = qw_u.parts64.msw;					        \
  (ix1) = qw_u.parts64.lsw;					        \
} while (0)

/* Set a long double from two 64 bit ints.  */

#define SET_LDOUBLE_WORDS64(d,ix0,ix1)				\
do {								                \
  ieee_quad_shape_type qw_u;					    \
  qw_u.parts64.msw = (ix0);					        \
  qw_u.parts64.lsw = (ix1);					        \
  (d) = qw_u.value;						            \
} while (0)

#define GET_LDOUBLE_MSW64(v,d)					    \
do {								                \
  ieee_quad_shape_type sh_u;					    \
  sh_u.value = (d);						            \
  (v) = sh_u.parts64.msw;					        \
} while (0)

/* Set the more significant 64 bits of a long double mantissa from an int.  */

#define SET_LDOUBLE_MSW64(d,v)					    \
do {								                \
  ieee_quad_shape_type sh_u;					    \
  sh_u.value = (d);						            \
  sh_u.parts64.msw = (v);					        \
  (d) = sh_u.value;						            \
} while (0)

/* Get the least significant 64 bits of a long double mantissa.  */

#define GET_LDOUBLE_LSW64(v,d)					    \
do {								                \
  ieee_quad_shape_type sh_u;					    \
  sh_u.value = (d);						            \
  (v) = sh_u.parts64.lsw;					        \
} while (0)

/* Get a 32 bit int from a float.  */

#define GET_FLOAT_WORD(i,d)		                    \
do {							                    \
  ieee_float_shape_type gf_u;	                    \
  gf_u.value = (d);				                    \
  (i) = gf_u.word;				                    \
} while (0)

/* Set a float from a 32 bit int.  */

#define SET_FLOAT_WORD(d,i)		                    \
do {							                    \
  ieee_float_shape_type sf_u;	                    \
  sf_u.word = (i);				                    \
  (d) = sf_u.value;				                    \
} while (0)

#define	STRICT_ASSIGN(type, lval, rval) do {	    \
	volatile type __lval;			                \
						                            \
	if (sizeof(type) >= sizeof(long double))	    \
		(lval) = (rval);		                    \
	else {					                        \
		__lval = (rval);		                    \
		(lval) = __lval;		                    \
	}					                            \
} while (0)

#define	TRUNC(d)	    (_b_trunc(&(d)))
#define	nan_mix(x, y)	(((x) + 0.0L) + ((y) + 0))

#define math_force_eval(x) \
({ __typeof (x) __x = (x); __asm __volatile ("" : : "m" (__x)); })

static __inline void
_b_trunc(volatile double *_dp) {
    uint32_t _lw;

    GET_LOW_WORD(_lw, *_dp);
    SET_LOW_WORD(*_dp, _lw & 0xf8000000);
}

/* If there are rounding modes other than FE_TONEAREST defined, then
 * add code to check which is active
 */
#if (defined(FE_UPWARD) + defined(FE_DOWNWARD) + defined(FE_TOWARDZERO)) >= 1
#define FE_DECL_ROUND(v)        int v = fegetround()
#define __is_nearest(r)         ((r) == FE_TONEAREST)
#else
#define FE_DECL_ROUND(v)
#define __is_nearest(r)         1
#endif

#ifdef FE_UPWARD
#define __is_upward(r)          ((r) == FE_UPWARD)
#else
#define __is_upward(r)          0
#endif

#ifdef FE_DOWNWARD
#define __is_downward(r)        ((r) == FE_DOWNWARD)
#else
#define __is_downward(r)        0
#endif

#ifdef FE_TOWARDZERO
#define __is_towardzero(r)      ((r) == FE_TOWARDZERO)
#else
#define __is_towardzero(r)      0
#endif

#ifdef _FLT_NO_DENORMALS
#define FLT_UWORD_IS_ZERO(x) ((x)<0x00800000L)
#define FLT_UWORD_IS_SUBNORMAL(x) 0
#define FLT_UWORD_MIN 0x00800000
#define FLT_UWORD_EXP_MIN 0x42fc0000
#define FLT_UWORD_LOG_MIN 0x42aeac50
#define FLT_SMALLEST_EXP 1
#else
#define FLT_UWORD_IS_ZERO(x) ((x)==0)
#define FLT_UWORD_IS_SUBNORMAL(x) ((x)<0x00800000L)
#define FLT_UWORD_MIN 0x00000001
#define FLT_UWORD_EXP_MIN 0x43160000
#define FLT_UWORD_LOG_MIN 0x42cff1b5
#define FLT_SMALLEST_EXP -22
#endif

#ifdef __SPE__
#define FLT_UWORD_IS_FINITE(x) 1
#define FLT_UWORD_IS_NAN(x) 0
#define FLT_UWORD_IS_INFINITE(x) 0
#define FLT_UWORD_MAX 0x7fffffff
#define FLT_UWORD_EXP_MAX 0x43010000
#define FLT_UWORD_LOG_MAX 0x42b2d4fc
#define FLT_UWORD_LOG_2MAX 0x42b437e0
#define HUGE ((float)0X1.FFFFFEP128)
#else
#define FLT_UWORD_IS_FINITE(x) ((x)<0x7f800000L)
#define FLT_UWORD_IS_NAN(x) ((x)>0x7f800000L)
#define FLT_UWORD_IS_INFINITE(x) ((x)==0x7f800000L)
#define FLT_UWORD_MAX 0x7f7fffffL
#define FLT_UWORD_EXP_MAX 0x43000000
#define FLT_UWORD_LOG_MAX 0x42b17217
#define FLT_UWORD_LOG_2MAX 0x42b2d4fc
#define HUGE ((float)3.40282346638528860e+38)
#endif

extern double   __kernel_cos(double x, double y);
extern double   __kernel_sin(double x, double y, int iy);
extern int      __ieee754_rem_pio2(double x, double *y);
extern int      __ieee754_rem_pio2f(float x, double *y);
extern int      __kernel_rem_pio2(double *x, double *y, int e0, int nx, int prec);
extern double   __kernel_tan(double x, double y, int iy);
extern float    __kernel_cosdf(double x);
extern float    __kernel_sindf(double x);
extern double   __ldexp_exp(double x, int expt);
extern float    __ldexp_expf(float,int);
extern float    __kernel_tandf(double x, int y);
extern float    lgammaf_r(float, int *);
extern double   lgamma_r(double, int *);
extern double complex __ldexp_cexp(double complex z, int expt);
extern float  complex __ldexp_cexpf(float complex,int);

#define	__ieee754_sqrt	        sqrt
#define	__ieee754_acos	        acos
#define	__ieee754_acosh	        acosh
#define	__ieee754_log	        log
#define	__ieee754_log2	        log2
#define	__ieee754_atanh	        atanh
#define	__ieee754_asin	        asin
#define	__ieee754_atan2	        atan2
#define	__ieee754_exp	        exp
#define	__ieee754_cosh	        cosh
#define	__ieee754_fmod	        fmod
#define	__ieee754_pow	        pow
#define	__ieee754_lgamma        lgamma
#define	__ieee754_lgamma_r      lgamma_r
#define	__ieee754_log10	        log10
#define	__ieee754_sinh	        sinh
#define	__ieee754_hypot	        hypot
#define	__ieee754_j0	        j0
#define	__ieee754_j1	        j1
#define	__ieee754_y0	        y0
#define	__ieee754_y1	        y1
#define	__ieee754_jn	        jn
#define	__ieee754_yn	        yn
#define	__ieee754_remainder     remainder
#define	__ieee754_sqrtf	        sqrtf
#define	__ieee754_acosf	        acosf
#define	__ieee754_acoshf        acoshf
#define	__ieee754_logf	        logf
#define	__ieee754_atanhf        atanhf
#define	__ieee754_asinf	        asinf
#define	__ieee754_atan2f        atan2f
#define	__ieee754_expf	        expf
#define	__ieee754_coshf	        coshf
#define	__ieee754_fmodf	        fmodf
#define	__ieee754_powf	        powf
#define	__ieee754_lgammaf       lgammaf
#define	__ieee754_lgammaf_r     lgammaf_r
#define	__ieee754_log10f        log10f
#define	__ieee754_log2f         log2f
#define	__ieee754_sinhf	        sinhf
#define	__ieee754_hypotf        hypotf
#define	__ieee754_j0f	        j0f
#define	__ieee754_j1f	        j1f
#define	__ieee754_y0f	        y0f
#define	__ieee754_y1f	        y1f
#define	__ieee754_jnf	        jnf
#define	__ieee754_ynf	        ynf
#define	__ieee754_remainderf    remainderf

#ifdef __SPE__
#include "soft-float/soft-fp.h"
#include "soft-float/double.h"
#include "soft-float/quad.h"
#include "soft-float/single.h"

extern SFtype __addsf3(SFtype a, SFtype b);
extern DFtype __adddf3(DFtype a, DFtype b);
extern DFtype __divdf3(DFtype a, DFtype b);
extern SFtype __divsf3(SFtype a, SFtype b);
extern TFtype __divtf3(TFtype a, TFtype b);
extern CMPtype __eqdf2(DFtype a, DFtype b);
extern CMPtype __eqsf2(SFtype a, SFtype b);
extern DFtype __extendsfdf2(SFtype a);
extern SItype __fixdfsi(DFtype a);
extern SItype __fixsfsi(SFtype a);
extern USItype __fixunsdfsi(DFtype a);
extern USItype __fixunssfsi(SFtype a);
extern DFtype __floatsidf(SItype i);
extern SFtype __floatsisf(SItype i);
extern DFtype __floatunsidf(USItype i);
extern SFtype __floatunsisf(USItype i);
extern CMPtype __gesf2(SFtype a, SFtype b);
extern CMPtype __gedf2(DFtype a, DFtype b);
extern CMPtype __gtdf2(DFtype a, DFtype b);
extern CMPtype __gtsf2(SFtype a, SFtype b);
extern CMPtype __ledf2(DFtype a, DFtype b);
extern CMPtype __lesf2(SFtype a, SFtype b);
extern CMPtype __ltdf2(DFtype a, DFtype b);
extern CMPtype __ltsf2(SFtype a, SFtype b);
extern DFtype __muldf3(DFtype a, DFtype b);
extern SFtype __mulsf3(SFtype a, SFtype b);
extern CMPtype __nedf2(DFtype a, DFtype b);
extern CMPtype __nesf2(SFtype a, SFtype b);
extern DFtype __subdf3(DFtype a, DFtype b);
extern SFtype __subsf3(SFtype a, SFtype b);
extern SFtype __truncdfsf2(DFtype a);
#endif

#endif /* _MATH_HEADERS_H */
