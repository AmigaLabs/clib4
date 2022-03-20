/*
 * $Id: wchar_fputwc.c,v 1.4 2021-02-03 18:49:27 clib2devs Exp $
*/

#ifndef _WCHAR_HEADERS_H
#include "wchar_headers.h"
#endif /* _WCHAR_HEADERS_H */

#include <stdlib.h>

wint_t
fputwc(wchar_t wc, FILE *fp)
{
    char buf[MB_LEN_MAX] = {0};
    size_t i, len;

    if (MB_CUR_MAX == 1 && wc > 0 && wc <= UCHAR_MAX)
    {
        /*
       * Assume single-byte locale with no special encoding.
       * A more careful test would be to check
       * _CurrentRuneLocale->encoding.
       */
        *buf = (unsigned char)wc;
        len = 1;
    }
    else
    {
        if ((len = wcrtomb(buf, wc, &fp->_mbstate)) == (size_t)-1)
        {
            fp->_flags2 |= __SERR;
            return WEOF;
        }
    }

    for (i = 0; i < len; i++)
        if (fputc((unsigned char)buf[i], fp) == EOF)
            return WEOF;

    return (wint_t)wc;
}
