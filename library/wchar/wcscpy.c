/*
 * $Id: wchar_wcscpy.c,v 1.3 2006-01-08 12:04:27 clib4devs Exp $
*/

#ifndef _WCHAR_HEADERS_H
#include "wchar_headers.h"
#endif /* _WCHAR_HEADERS_H */

wchar_t *
wcscpy(wchar_t *dest, const wchar_t *src) {
    ENTER();

    wchar_t *a = dest;
    while ((*dest++ = *src++));

    LEAVE();
    return a;
}
