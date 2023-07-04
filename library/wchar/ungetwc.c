/*
 * $Id: wchar_ungetwc.c,v 1.3 2006-01-08 12:04:27 clib2devs Exp $
*/

#ifndef _WCHAR_HEADERS_H
#include "wchar_headers.h"
#endif /* _WCHAR_HEADERS_H */

wint_t
ungetwc(wint_t wc, FILE *fp) {
    char buf[MB_LEN_MAX];
    size_t len;

    ENTER();

    flockfile(fp);
    ORIENT (fp, 1);
    if (wc != WEOF)
    {
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
    funlockfile(fp);

    return wc;
}
