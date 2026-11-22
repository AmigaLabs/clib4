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

    return _mbstowcs_r(pwcs, s, n, &state);
}
