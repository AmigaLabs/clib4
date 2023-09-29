/*
 * $Id: wchar_wcsrchr.c,v 1.0 2021-01-15 00:42:27 clib4devs Exp $
*/

#ifndef _WCHAR_HEADERS_H
#include "wchar_headers.h"
#endif /* _WCHAR_HEADERS_H */

/* Find the last occurrence of WC in WCS.  */
wchar_t *
wcsrchr(const wchar_t *wcs, const wchar_t wc) {
    const wchar_t *p;

    ENTER();

    p = wcs;
    while (*p)
        p++;

    while (wcs <= p) {
        if (*p == wc) {
            /* LINTED interface specification */
            LEAVE();
            return (wchar_t *) p;
        }
        p--;
    }

    LEAVE();
    return NULL;
}