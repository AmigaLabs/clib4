/*
 * $Id: wchar_wctob.c,v 1.4 2021-02-03 19:11:16 clib2devs Exp $
*/

#ifndef _WCHAR_HEADERS_H
#include "wchar_headers.h"
#endif /* _WCHAR_HEADERS_H */

int
wctob(wint_t c) {
    mbstate_t mbs;
    int retval = 0;
    char pwc;

    ENTER();

    /* Put mbs in initial state. */
    memset(&mbs, '\0', sizeof(mbs));

    retval = _wctomb_r(&pwc, c, &mbs);

    if (c == EOF || retval != 1) {
        RETURN(WEOF);
        return WEOF;
    }
    else {
        RETURN((int)pwc);
        return (int) pwc;
    }
}
