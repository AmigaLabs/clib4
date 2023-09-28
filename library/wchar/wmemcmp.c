/*
 * $Id: wchar_wmemcmp.c,v 1.3 2006-01-08 12:04:27 clib4devs Exp $
*/

#ifndef _WCHAR_HEADERS_H
#include "wchar_headers.h"
#endif /* _WCHAR_HEADERS_H */

int
wmemcmp(const wchar_t *ptr1, const wchar_t *ptr2, size_t len) {
    size_t i;

    for (i = 0; i < len; i++) {
        if (*ptr1 != *ptr2) {
            /* wchar might be unsigned */
            return *ptr1 > *ptr2 ? 1 : -1;
        }
        ptr1++;
        ptr2++;
    }

    return 0;
}
