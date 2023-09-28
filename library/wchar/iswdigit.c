/*
 * $Id: wctype_iswdigit.c,v 1.4 2021-02-04 00:00:27 clib4devs Exp $
*/

#ifndef _WCHAR_HEADERS_H
#include "wchar_headers.h"
#endif /* _WCHAR_HEADERS_H */

#ifndef _WCTYPE_HEADERS_H
#include <wctype.h>
#endif /* _WCTYPE_HEADERS_H */

#include <ctype.h>

int
iswdigit(wint_t c) {
    ENTER();

    int result = (unsigned) c - '0' < 10;

    RETURN(result);
    return result;
}
