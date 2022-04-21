/*
 * $Id: wchar_wcstold.c,v 1.0 2021-02-04 12:15:27 clib2devs Exp $
*/

#ifndef _WCHAR_HEADERS_H
#include "wchar_headers.h"
#endif /* _WCHAR_HEADERS_H */

#ifndef _WCTYPE_HEADERS_H
#include <wctype.h>
#endif /* _WCTYPE_HEADERS_H */

long double
wcstold(const wchar_t *nptr, wchar_t **endptr)
{
    ENTER();

    /* On platforms where long double is as wide as double.  */
    return wcstod(nptr, endptr);
}