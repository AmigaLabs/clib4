/*
 * $Id: wchar_wcstof.c,v 1.0 2021-09-28 12:04:27 clib2devs Exp $
*/

#ifndef _WCHAR_HEADERS_H
#include "wchar_headers.h"
#endif /* _WCHAR_HEADERS_H */

#ifndef _WCTYPE_HEADERS_H
#include <wctype.h>
#endif /* _WCTYPE_HEADERS_H */

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

float
wcstof(const wchar_t *nptr, wchar_t **endptr) {
    double val = wcstod(nptr, endptr);
    if (isnan(val))
        return nanf("");

    ENTER();

    float retval = (float) val;
    if (isinf(retval) && !isinf(val))
        __set_errno(ERANGE);

    RETURN(retval);
    return retval;
}
