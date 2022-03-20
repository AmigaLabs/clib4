/*
 * $Id: stdlib_mbtowc.c,v 1.3 2006-01-08 12:04:26 clib2devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#include "wchar_headers.h"

int
mbtowc(wchar_t *wc, const char *src, size_t n) {
    int retval = 0;
    mbstate_t *ps = &__global_clib2->wide_status->_mbtowc_state;

    retval = _mbtowc(wc, src, n, ps);

    if (retval < 0) {
        ps->__count = 0;
        return -1;
    }
    return retval;
}
