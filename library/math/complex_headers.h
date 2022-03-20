/*
 * $Id: complex_headers.h,v 1.5 2006-01-30 12:04:22 clib2devs Exp $
*/

#ifndef _COMPLEX_HEADERS_H
#define _COMPLEX_HEADERS_H

/****************************************************************************/

#ifndef _MATH_FP_SUPPORT_H
#include "math_fp_support.h"
#endif /* _MATH_FP_SUPPORT_H */

/****************************************************************************/

#if defined(__GNUC__) && defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)

/****************************************************************************/

#include <complex.h>
#include <float.h>
#include <math.h>

/****************************************************************************/

#define	REAL(z)	(__real__ (z))
#define	IMAG(z)	(__imag__ (z))
#define	CONJ(z)	(~(z))

/****************************************************************************/

#endif /* __GNUC__ && __STDC_VERSION__ */

/****************************************************************************/

#endif /* _COMPLEX_HEADERS_H */
