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

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#include "dla.h"

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
#   define HIGH_HALF 1
#   define LOW_HALF  0
#  endif
#endif

typedef union {
	double value;
	struct 
	{
		unsigned int msw;
		unsigned int lsw;
	} parts;
} ieee_double_shape_type;

typedef union {
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

/* A union which permits us to convert between a float and a 32 bit
   int. */

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

typedef int int4;
typedef union {int4 i[2]; double x;} mynumber;

#define EXTRACT_WORDS(ix0,ix1,d)					\
do {												\
  ieee_double_shape_type ew_u;						\
  ew_u.value = (d);									\
  (ix0) = ew_u.parts.msw;							\
  (ix1) = ew_u.parts.lsw;							\
} while (0)

/* Get the more significant 32 bit int from a double.  */

#define GET_HIGH_WORD(i,d)							\
do {												\
  ieee_double_shape_type gh_u;						\
  gh_u.value = (d);									\
  (i) = gh_u.parts.msw;								\
} while (0)

/* Get the less significant 32 bit int from a double.  */

#define GET_LOW_WORD(i,d)							\
do {												\
  ieee_double_shape_type gl_u;						\
  gl_u.value = (d);									\
  (i) = gl_u.parts.lsw;								\
} while (0)

/* Set a double from two 32 bit ints.  */

#define INSERT_WORDS(d,ix0,ix1)						\
do {												\
  ieee_double_shape_type iw_u;						\
  iw_u.parts.msw = (ix0);							\
  iw_u.parts.lsw = (ix1);							\
  (d) = iw_u.value;									\
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
do {								\
  ieee_extended_shape_type ew_u;				\
  ew_u.value = (d);						\
  (se) = ew_u.parts.exp;					\
  (ix0) = ew_u.parts.msw;					\
  (ix1) = ew_u.parts.lsw;					\
} while (0)

/* Set a double from two 32 bit ints.  */

#define SET_LDOUBLE_WORDS(d,se,ix0,ix1)				\
do {								\
  ieee_extended_shape_type iw_u;				\
  iw_u.parts.exp = (se);					\
  iw_u.parts.msw = (ix0);					\
  iw_u.parts.lsw = (ix1);					\
  (d) = iw_u.value;						\
} while (0)

/* Get the more significant 32 bits of a long double mantissa.  */

#define GET_LDOUBLE_MSW(v,d)					\
do {								\
  ieee_extended_shape_type sh_u;				\
  sh_u.value = (d);						\
  (v) = sh_u.parts.msw;						\
} while (0)

/* Set the more significant 32 bits of a long double mantissa from an int.  */

#define SET_LDOUBLE_MSW(d,v)					\
do {								\
  ieee_extended_shape_type sh_u;				\
  sh_u.value = (d);						\
  sh_u.parts.msw = (v);						\
  (d) = sh_u.value;						\
} while (0)

/* Get int from the exponent of a long double.  */

#define GET_LDOUBLE_EXP(se,d)					\
do {								\
  ieee_extended_shape_type ge_u;				\
  ge_u.value = (d);						\
  (se) = ge_u.parts.exp;					\
} while (0)

/* Set exponent of a long double from an int.  */

#define SET_LDOUBLE_EXP(d,se)					\
do {								\
  ieee_extended_shape_type se_u;				\
  se_u.value = (d);						\
  se_u.parts.exp = (se);					\
  (d) = se_u.value;						\
} while (0)

/* Get two 64 bit ints from a long double.  */

#define GET_LDOUBLE_WORDS64(ix0,ix1,d)				\
do {								\
  ieee_quad_shape_type qw_u;					\
  qw_u.value = (d);						\
  (ix0) = qw_u.parts64.msw;					\
  (ix1) = qw_u.parts64.lsw;					\
} while (0)

/* Set a long double from two 64 bit ints.  */

#define SET_LDOUBLE_WORDS64(d,ix0,ix1)				\
do {								\
  ieee_quad_shape_type qw_u;					\
  qw_u.parts64.msw = (ix0);					\
  qw_u.parts64.lsw = (ix1);					\
  (d) = qw_u.value;						\
} while (0)

#define GET_LDOUBLE_MSW64(v,d)					\
do {								\
  ieee_quad_shape_type sh_u;					\
  sh_u.value = (d);						\
  (v) = sh_u.parts64.msw;					\
} while (0)

/* Set the more significant 64 bits of a long double mantissa from an int.  */

#define SET_LDOUBLE_MSW64(d,v)					\
do {								\
  ieee_quad_shape_type sh_u;					\
  sh_u.value = (d);						\
  sh_u.parts64.msw = (v);					\
  (d) = sh_u.value;						\
} while (0)

/* Get the least significant 64 bits of a long double mantissa.  */

#define GET_LDOUBLE_LSW64(v,d)					\
do {								\
  ieee_quad_shape_type sh_u;					\
  sh_u.value = (d);						\
  (v) = sh_u.parts64.lsw;					\
} while (0)

/* Get a 32 bit int from a float.  */

#define GET_FLOAT_WORD(i,d)		\
do {							\
  ieee_float_shape_type gf_u;	\
  gf_u.value = (d);				\
  (i) = gf_u.word;				\
} while (0)

/* Set a float from a 32 bit int.  */

#define SET_FLOAT_WORD(d,i)		\
do {							\
  ieee_float_shape_type sf_u;	\
  sf_u.word = (i);				\
  (d) = sf_u.value;				\
} while (0)

#define	STRICT_ASSIGN   (type, lval, rval)	((lval) = (rval))
#define	TRUNC(d)	    (_b_trunc(&(d)))
#define	nan_mix(x, y)	(((x) + 0.0L) + ((y) + 0))

#define math_force_eval(x) \
({ __typeof (x) __x = (x); __asm __volatile ("" : : "m" (__x)); })

static __inline void
_b_trunc(volatile double *_dp) {
    //VBS
    //u_int32_t _lw;
    uint32_t _lw;

    GET_LOW_WORD(_lw, *_dp);
    SET_LOW_WORD(*_dp, _lw & 0xf8000000);
}

extern double __kernel_cos(double x, double y);
extern double __kernel_sin(double x, double y, int iy);
extern int __rem_pio2(double x, double *y);
extern double __kernel_tan(double x, double y, int iy);
extern double __expm1(double x);
extern float __kernel_cosf(float x, float y);
extern float __kernel_cosdf(double x);
extern float __kernel_sinf(float x, float y, int iy);
extern float __kernel_sindf(double x);
extern int __rem_pio2f(float x, double *y);
extern float __kernel_tanf(float x, float y, int iy);
extern float __kernel_tandf(double x, int iy);
extern double __lgamma(double x,int * gamma_sign_ptr);
extern float __lgammaf(float x,int * gamma_sign_ptr);
extern int __kernel_rem_pio2(double *x, double *y, int e0, int nx, int prec);
extern double __ldexp_exp(double x, int expt);
extern float __ldexp_expf(float,int);
extern double complex __ldexp_cexp(double complex z, int expt);
extern float complex __ldexp_cexpf(float complex,int);

#endif /* _MATH_HEADERS_H */
