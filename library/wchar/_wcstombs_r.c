/*
 * $Id: _wcstombs_r.c,v 1.0 2022-01-05 16:12:27 clib4devs Exp $
*/

#ifndef _WCHAR_HEADERS_H
#include "wchar_headers.h"
#endif /* _WCHAR_HEADERS_H */

size_t
_wcstombs_r(char *s, const wchar_t *pwcs, size_t n, mbstate_t *state) {
    char *ptr = s;
    size_t max = n;
    char buff[8] = {0};
    size_t i, num_to_copy;

    while (n > 0) {
        int bytes = _wctomb_r(buff, *pwcs, state);
        if (bytes == -1)
            return -1;
        num_to_copy = (n > (size_t) bytes ? (size_t) bytes : n);
        for (i = 0; (size_t) i < num_to_copy; ++i)
            *ptr++ = buff[i];

        if (*pwcs == 0x00)
            return ptr - s - (n >= (size_t) bytes);
        ++pwcs;
        n -= num_to_copy;
    }

    return max;
}