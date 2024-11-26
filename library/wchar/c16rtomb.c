/*
 * $Id: wchar_c16rtomb.c,v 1.0 2022-08-04 19:11:16 clib4devs Exp $
*/

#ifndef _WCHAR_HEADERS_H
#include "wchar_headers.h"
#endif /* _WCHAR_HEADERS_H */

#include <uchar.h>

size_t
c16rtomb(char *s, char16_t c16, mbstate_t *ps) {
    static unsigned internal_state;
    if (!ps) ps = (void *) &internal_state;
    unsigned *x = (unsigned *) ps;
    wchar_t wc;

    if (!s) {
        if (*x) goto ilseq;
        return 1;
    }

    if (!*x && c16 - 0xd800u < 0x400) {
        *x = c16 - 0xd7c0 << 10;
        return 0;
    }

    if (*x) {
        if (c16 - 0xdc00u >= 0x400) goto ilseq;
        else wc = *x + c16 - 0xdc00;
        *x = 0;
    } else {
        wc = c16;
    }
    return wcrtomb(s, wc, 0);

ilseq:
    *x = 0;
    __set_errno(EILSEQ);
    return -1;
}
