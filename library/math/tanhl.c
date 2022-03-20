/*
 * $Id: math_tanhl.c,v 1.2 2022-03-10 19:28:23 clib2devs Exp $
 *

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
 * Conversion to float by Ian Lance Taylor, Cygnus Support, ian@cygnus.com.
 */

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

long double
tanhl(long double x) {
    return tanh(x);
}
