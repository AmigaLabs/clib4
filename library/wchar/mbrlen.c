/*
 * $Id: wchar_mbrlen.c,v 1.4 2021-02-03 19:11:16 clib4devs Exp $
*/

#ifndef _WCHAR_HEADERS_H
#include "wchar_headers.h"
#endif /* _WCHAR_HEADERS_H */

size_t
mbrlen(const char *s, size_t n, mbstate_t *ps) {
    struct _clib4 *__clib4 = __CLIB4;

    if (ps == NULL) {
        ps = &__clib4->wide_status->_mbrlen_state;
    }

    return mbrtowc(NULL, s, n, ps);
}
