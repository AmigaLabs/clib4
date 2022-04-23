/*
 * $Id: wchar_fgetwc.c,v 1.3 2006-01-08 12:04:27 clib2devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#ifndef _WCHAR_HEADERS_H
#include "wchar_headers.h"
#endif /* _WCHAR_HEADERS_H */

wint_t
__fgetwc_unlocked_internal(FILE *f) {
    ENTER();

    wchar_t wc;
    int c;
    size_t l = 0;
    unsigned char b;
    int first = 1;

    do {
        b = c = __getc(f);
        if (c < 0) {
            if (!first) {
                f->_flags2 |= __SERR;
                __set_errno(EILSEQ);
            }
            RETURN(WEOF);
            return WEOF;
        }
        l = mbrtowc(&wc, (void *) &b, 1, &f->_mbstate);
        if (l == -1) {
            if (!first) {
                f->_flags2 |= __SERR;
                ungetc(b, f);
            }
            RETURN(WEOF);
            return WEOF;
        }
        first = 0;
    } while (l == -2);

    RETURN(wc);
    return wc;
}

wint_t
__fgetwc_unlocked(FILE *f) {
    if ((f->_flags2 & __SWID) <= 0)
        fwide(f, 1);

    wchar_t wc = __fgetwc_unlocked_internal(f);

    return wc;
}

wint_t
fgetwc(FILE *f) {
    wint_t c;

    flockfile(f);
    c = __fgetwc_unlocked(f);
    funlockfile(f);

    return c;
}
