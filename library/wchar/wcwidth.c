/*
 * $Id: wchar_wcwidth.c,v 1.0 2021-02-21 22:50:26 clib2devs Exp $
*/

#ifndef _WCHAR_HEADERS_H
#include "wchar_headers.h"
#endif /* _WCHAR_HEADERS_H */

#include <wctype.h>

int
wcwidth(const wchar_t wc) {
    int result = -1;
    ENTER();

    if (iswprint(wc))
        result = 1;
    else if (iswcntrl(wc) || wc == L'\0')
        result = 0;

    RETURN(result);
    return result;
}
