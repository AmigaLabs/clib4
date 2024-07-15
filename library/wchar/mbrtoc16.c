/*
 * $Id: wchar_mbrtoc16.c,v 1.0 2022-08-04 19:11:16 clib4devs Exp $
*/

#ifndef _WCHAR_HEADERS_H
#include "wchar_headers.h"
#endif /* _WCHAR_HEADERS_H */

#include <uchar.h>

size_t
mbrtoc16(char16_t *pc16, const char *s, size_t n, mbstate_t *ps) {
    static unsigned internal_state;
    if (!ps) ps = (void *) &internal_state;
    unsigned *pending = (unsigned *) ps;

    if (!s) return mbrtoc16(0, "", 1, ps);

    /* mbrtowc states for partial UTF-8 characters have the high bit set;
     * we use nonzero states without high bit for pending surrogates. */
    if ((int) *pending > 0) {
        if (pc16) *pc16 = *pending;
        *pending = 0;
        return -3;
    }

    wchar_t wc;
    size_t ret = mbrtowc(&wc, s, n, ps);
    if (ret <= 4) {
        if (wc >= 0x10000) {
            *pending = (wc & 0x3ff) + 0xdc00;
            wc = 0xd7c0 + (wc >> 10);
        }
        if (pc16) *pc16 = wc;
    }
    return ret;
}
