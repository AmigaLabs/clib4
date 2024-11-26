/*
 * $Id: wchar_ungetwc.c,v 1.4 2024-07-20 12:04:27 clib4devs Exp $
*/

#ifndef _WCHAR_HEADERS_H
#include "wchar_headers.h"
#endif /* _WCHAR_HEADERS_H */

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

wint_t
ungetwc(wint_t wc, FILE *fp) {
    char buf[MB_LEN_MAX] = {0};
    size_t len;
    struct _clib4 *__clib4 = __CLIB4;

    ENTER();

    __flockfile_r(__clib4, fp);
    ORIENT (fp, 1);
    if (wc != WEOF) {
        if ((len = wcrtomb(buf, wc, &fp->_mbstate)) == (size_t) - 1) {
            fp->_flags |= __SERR;
            wc = WEOF;
        } else {
            while (len-- != 0) {
                if (ungetc((unsigned char) buf[len], fp) == EOF) {
                    wc = WEOF;
                    break;
                }
            }
        }
    }
    __funlockfile_r(__clib4, fp);

    LEAVE();
    return wc;
}
