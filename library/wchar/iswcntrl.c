/*
 * $Id: wctype_iswcntrl.c,v 1.4 2021-02-03 23:58:27 clib4devs Exp $
*/

#ifndef _WCHAR_HEADERS_H
#include "wchar_headers.h"
#endif /* _WCHAR_HEADERS_H */

#ifndef _WCTYPE_HEADERS_H
#include <wctype.h>
#endif /* _WCTYPE_HEADERS_H */

#include <ctype.h>

int
iswcntrl(wint_t c) {
    int unicode = 0;
    int result;
    struct _clib4 *__clib4 = __CLIB4;

    ENTER();

    if (__clib4->_current_locale[0] == 'C' && __clib4->_current_locale[1] == '\0') {
        unicode = 0;
        /* fall-through */
    } else if (!strcmp(__clib4->_current_locale, "C-JIS")) {
        c = __jp2uc(c, JP_JIS);
        unicode = 1;
    } else if (!strcmp(__clib4->_current_locale, "C-SJIS")) {
        c = __jp2uc(c, JP_SJIS);
        unicode = 1;
    } else if (!strcmp(__clib4->_current_locale, "C-EUCJP")) {
        c = __jp2uc(c, JP_EUCJP);
        unicode = 1;
    } else if (!strcmp(__clib4->_current_locale, "C-UTF-8")) {
        unicode = 1;
    }

    if (unicode) {
        result = ((c >= 0x0000 && c <= 0x001f) ||
                 (c >= 0x007f && c <= 0x009f) ||
                 c == 0x2028 || c == 0x2029);

        RETURN(result);
        return result;
    }


    result = (c < 0x100 ? iscntrl(c) : 0);
    RETURN(result);
    return result;
}
