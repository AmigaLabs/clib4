/*
 * $Id: stdlib_mbstowcs.c,v 1.4 2022-05-01 12:04:26 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

size_t
mbstowcs(wchar_t *pwcs, const char *s, size_t n) {
    mbstate_t state;
    state.__count = 0;

    wchar_t *ptr = pwcs;
    size_t max = n;
    char *t = (char *) s;
    int bytes;

    while (n > 0) {
        bytes = _mbtowc_r(ptr, t, MB_CUR_MAX, &state);
        if (bytes < 0) {
            state.__count = 0;
            return -1;
        } else if (bytes == 0) {
            return ptr - pwcs;
        }
        t += bytes;
        ++ptr;
        --n;
    }

    return max;
}
