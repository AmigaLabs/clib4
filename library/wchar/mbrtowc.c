/*
 * $Id: wchar_mbrlen.c,v 1.4 2021-02-03 19:11:16 clib2devs Exp $
*/

#ifndef _WCHAR_HEADERS_H
#include "wchar_headers.h"
#endif /* _WCHAR_HEADERS_H */

size_t
mbrtowc(wchar_t *pwc, const char *src, size_t n, mbstate_t *ps) {
    int retval = 0;

    ENTER();

    if (ps == NULL) {
        ps = &__CLIB2->wide_status->_mbrtowc_state;
    }

    if (src == NULL) {
        retval = _mbtowc_r(NULL, "", 1, ps);
    } else {
        retval = _mbtowc_r(pwc, src, n, ps);
    }

    if (retval == -1) {
        ps->__count = 0;
        __set_errno(EILSEQ);
        RETURN(-1);
        return (size_t)(-1);
    } else {
        RETURN(retval);
        return (size_t) retval;
    }
}
