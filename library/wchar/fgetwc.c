/*
 * $Id: wchar_fgetwc.c,v 1.3 2006-01-08 12:04:27 clib4devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#ifndef _WCHAR_HEADERS_H
#include "wchar_headers.h"
#endif /* _WCHAR_HEADERS_H */

wint_t
__fgetwc_unlocked_internal(struct _clib4 *__clib4, FILE *f) {
    ENTER();

    wchar_t wc;
    int c;
    size_t l = 0;
    unsigned char b;
    int first = 1;

    do {
        b = c = __getc(__clib4, f);
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
                __ungetc_r(__clib4, b, f);
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
__fgetwc_unlocked(struct _clib4 *__clib4, FILE *f) {
    if ((f->_flags2 & __SWID) <= 0)
        fwide(f, 1);

    wchar_t wc = __fgetwc_unlocked_internal(__clib4, f);

    return wc;
}

wint_t
fgetwc(FILE *f) {
    wint_t c;
    struct _clib4 *__clib4 = __CLIB4;

    __check_abort_f(__clib4);

    __flockfile_r(__clib4, f);

    c = __fgetwc_unlocked(__clib4, f);

    __funlockfile_r(__clib4, f);

    return c;
}
