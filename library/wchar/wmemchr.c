/*
 * $Id: wchar_wmemchr.c,v 1.4 2021-02-04 11:16:27 clib4devs Exp $
*/

#ifndef _WCHAR_HEADERS_H
#include "wchar_headers.h"
#endif /* _WCHAR_HEADERS_H */

wchar_t *
wmemchr(const wchar_t *ptr, wchar_t val, size_t len) {
    size_t i;

    if (ptr == NULL) {
        return NULL;
    }

    for (i = 0; i < len; i++) {
        if (*ptr == val) {
            /* LINTED const castaway */
            return (wchar_t *) ptr;
        }
        ptr++;
    }
    return NULL;
}
