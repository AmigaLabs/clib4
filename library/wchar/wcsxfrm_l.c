/*
 * $Id: wchar_wcsxfrm_l.c,v 1.1 2022-03-05 19:34:27 clib4devs Exp $
*/

#ifndef _WCHAR_HEADERS_H
#include "wchar_headers.h"
#endif /* _WCHAR_HEADERS_H */

size_t
wcsxfrm_l(wchar_t *dest, const wchar_t *src, size_t len, locale_t loc) {
    (void) loc;

    return wcsxfrm(dest, src, len);
}
