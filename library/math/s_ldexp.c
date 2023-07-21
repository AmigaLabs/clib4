/*
 * $Id: math_s_ldexp.c,v 1.7 2023-07-19 12:04:23 clib2devs Exp $
*/

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

double
ldexp(double x, int exp) {
    double result;

    if (isinf(x) || fpclassify(x) == FP_ZERO) {
        result = x;
    } else {
        result = scalbn(x, exp);

        if (isinf(result) || (result < DBL_MIN || result > -DBL_MIN))
            __set_errno(ERANGE);
    }

    return (result);
}
