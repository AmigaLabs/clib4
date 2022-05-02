/*
 * $Id: stdlib_wcstombs.c,v 1.4 2022-01-05 12:04:26 clib2devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

size_t
wcstombs(char *s, const wchar_t *pwcs, size_t n) {
    mbstate_t state;
    state.__count = 0;

    return _wcstombs_r(s, pwcs, n, &state);
}
