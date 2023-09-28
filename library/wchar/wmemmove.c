/*
 * $Id: wchar_wmemmove.c,v 1.4 2021-02-04 11:13:27 clib4devs Exp $
*/

#ifndef _WCHAR_HEADERS_H
#include "wchar_headers.h"
#endif /* _WCHAR_HEADERS_H */

wchar_t *
wmemmove(wchar_t *dest, const wchar_t *src, size_t len) {
    return (wchar_t *) memmove(dest, src, len * sizeof(wchar_t));
}
