/*
 * $Id: math_e_remainderl.c,v 1.4 2023-07-19 12:04:24 clib4devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

long double
remainderl(long double x, long double p) {
    return remainder(x, p);
}
