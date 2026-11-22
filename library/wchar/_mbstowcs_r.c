/*
 * $Id: _mbstowcs_r.c,v 1.0 2022-01-05 16:12:27 clib4devs Exp $
*/

#ifndef _WCHAR_HEADERS_H
#include "wchar_headers.h"
#endif /* _WCHAR_HEADERS_H */

size_t
_mbstowcs_r(wchar_t *pwcs, const char *s, size_t n, mbstate_t *state) {
    size_t count = 0;
    char *t = (char *) s;
    int bytes;

    /* Without a destination, count the whole string: n doesn't matter */
    if (pwcs == NULL)
        n = 1;

    while (n > 0) {
        bytes = _mbtowc_r(pwcs, t, MB_CUR_MAX, state);
        if (bytes < 0) {
            state->__count = 0;
            return -1;
        } else if (bytes == 0)
            break;
        t += bytes;
        ++count;
        if (pwcs != NULL) {
            ++pwcs;
            --n;
        }
    }

    return count;
}
