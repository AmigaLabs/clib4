/*
 * $Id: wchar_wmemset.c,v 1.4 2021-02-04 11:15:27 clib4devs Exp $
*/

#ifndef _WCHAR_HEADERS_H
#include "wchar_headers.h"
#endif /* _WCHAR_HEADERS_H */

wchar_t *
wmemset(wchar_t *ptr, int val, size_t len) {
    size_t i;
    wchar_t *p;

    p = (wchar_t *) ptr;
    for (i = 0; i < len; i++) {
        *p = val;
        p++;
    }

    return ptr;
}
