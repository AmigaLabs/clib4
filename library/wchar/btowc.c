/*
 * $Id: wchar_btowc.c,v 1.4 2021-02-03 19:11:16 clib2devs Exp $
*/

#ifndef _WCHAR_HEADERS_H
#include "wchar_headers.h"
#endif /* _WCHAR_HEADERS_H */

#include "wchar_wprintf_core.h"

wint_t
btowc(int c) {
    struct _clib2 *__clib2 = __CLIB2;

    mbstate_t *mbs = &__clib2->wide_status->_mbtowc_state;
    int retval = 0;
    wchar_t pwc;
    char b;

    b = (char) c;

    /* Put mbs in initial state. */
    memset(mbs, '\0', sizeof(mbstate_t));

    retval = _mbtowc_r(&pwc, &b, 1, mbs);

    if (c == EOF || retval != 1) {
        return WEOF;
    } else {
        return (wint_t) pwc;
    }
}
