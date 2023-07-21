/*
 * $Id: math_s_nanl.c,v 1.5 2023-07-19 12:04:24 clib2devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

long double
nanl(const char *tagp) {
    return nan(tagp);
}
