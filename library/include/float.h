/*
 * $Id: float.h,v 1.5 2006-01-08 12:06:14 clib4devs Exp $
*/

#ifndef _FLOAT_H
#define _FLOAT_H

#include <features.h>

#define FLT_EPSILON		1.19209290E-07F
#define FLT_DIG			6
#define FLT_MANT_DIG	24
#define FLT_MAX			3.40282347E+38F
#define FLT_MAX_EXP		128
#define FLT_MAX_10_EXP	38
#define FLT_MIN			1.17549435E-38F
#define FLT_MIN_EXP		(-125)
#define FLT_MIN_10_EXP	(-37)
#define FLT_RADIX		2
#define FLT_ROUNDS		1

#define DBL_EPSILON		2.2204460492503131E-16
#define DBL_DIG			15
#define DBL_MANT_DIG	53
#define DBL_MAX			1.7976931348623157E+308
#define DBL_MAX_EXP		1024
#define DBL_MAX_10_EXP	308
#define DBL_MIN			2.2250738585072014E-308
#define DBL_MIN_EXP		(-1021)
#define DBL_MIN_10_EXP	(-307)

#define LDBL_TRUE_MIN       4.94065645841246544177e-324L
#define LDBL_EPSILON        DBL_EPSILON
#define LDBL_DIG		    DBL_DIG
#define LDBL_MANT_DIG	    DBL_MANT_DIG
#define LDBL_MAX		    DBL_MAX
#define LDBL_MAX_EXP	    DBL_MAX_EXP
#define LDBL_MAX_10_EXP	    DBL_MAX_10_EXP
#define LDBL_MIN		    DBL_MIN
#define LDBL_MIN_EXP	    DBL_MIN_EXP
#define LDBL_MIN_10_EXP	    DBL_MIN_10_EXP
#define LDBL_INF_NAN_EXP    2047

/****************************************************************************/

#if LDBL_MANT_DIG == DBL_MANT_DIG && LDBL_MAX_EXP == DBL_MAX_EXP
#define _LDBL_EQ_DBL 1
#endif

#endif /* _FLOAT_H */
