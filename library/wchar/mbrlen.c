/*
 * $Id: wchar_mbrlen.c,v 1.4 2021-02-03 19:11:16 clib2devs Exp $
*/

#ifndef _WCHAR_HEADERS_H
#include "wchar_headers.h"
#endif /* _WCHAR_HEADERS_H */

size_t
mbrlen(const char *restrict s, size_t n, mbstate_t *restrict ps) {
    if (ps == NULL) {
        ps = &__getclib2()->wide_status->_mbrlen_state;
    }

    return mbrtowc(NULL, s, n, ps);
}
