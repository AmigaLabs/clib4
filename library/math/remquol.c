/*
 * $Id: math_remquol.c,v 1.0 2022-03-12 12:04:24 clib2devs Exp $
 *

 *
 *
 * Copyright (C) 2002 by  Red Hat, Incorporated. All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software
 * is freely granted, provided that this notice is preserved.
 */

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

long double
remquol(long double x, long double y, int *quo) {
    return remquo(x, y, quo);
}
