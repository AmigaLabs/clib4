/*
 * $Id: wchar_btowc.c,v 1.4 2021-02-03 19:11:16 clib2devs Exp $
*/

#ifndef _WCHAR_HEADERS_H
#include "wchar_headers.h"
#endif /* _WCHAR_HEADERS_H */

#include "wchar_wprintf_core.h"

wint_t
btowc(int c) {
    ENTER();

    mbstate_t *mbs = &__global_clib2->wide_status->_mbtowc_state;
    int retval = 0;
    wchar_t pwc;
    char b;

    b = (char) c;

    /* Put mbs in initial state. */
    memset(mbs, '\0', sizeof(mbs));

    retval = _mbtowc(&pwc, &b, 1, mbs);

    if (c == EOF || retval != 1) {
        RETURN(WEOF);
        return WEOF;
    } else {
        RETURN((wint_t) pwc);
        return (wint_t) pwc;
    }
}
