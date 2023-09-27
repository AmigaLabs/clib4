/*
 * $Id: wchar_wcsxfrm.c,v 1.1 2022-03-05 19:34:27 clib4devs Exp $
*/

#ifndef _WCHAR_HEADERS_H
#include "wchar_headers.h"
#endif /* _WCHAR_HEADERS_H */

size_t
wcsxfrm(wchar_t *dest, const wchar_t *src, size_t len) {
    ENTER();

    if (*src == L'\0') {
        if (len != 0)
            *dest = L'\0';
        RETURN(0);
        return (0);
    }

    size_t l = wcslen(src);
    if (l < len) {
        wmemcpy(dest, src, l + 1);
    } else if (len) {
        wmemcpy(dest, src, len - 1);
        dest[len - 1] = 0;
    }

    RETURN(l);
    return l;
}
