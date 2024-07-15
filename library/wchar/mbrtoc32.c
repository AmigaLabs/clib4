/*
 * $Id: wchar_mbrtoc32.c,v 1.0 2022-08-04 19:11:16 clib4devs Exp $
*/

#ifndef _WCHAR_HEADERS_H
#include "wchar_headers.h"
#endif /* _WCHAR_HEADERS_H */

#include <uchar.h>

size_t
mbrtoc32(char32_t *pc32, const char *s, size_t n, mbstate_t *ps) {
    static unsigned internal_state;
    if (!ps) ps = (void *) &internal_state;
    if (!s) return mbrtoc32(0, "", 1, ps);
    wchar_t wc;
    size_t ret = mbrtowc(&wc, s, n, ps);
    if (ret <= 4 && pc32) *pc32 = wc;
    return ret;
}
