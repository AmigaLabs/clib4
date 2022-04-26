/*
 * $Id: wchar_fgetws.c,v 1.3 2006-01-08 12:04:27 clib2devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#ifndef _WCHAR_HEADERS_H
#include "wchar_headers.h"
#endif /* _WCHAR_HEADERS_H */

wchar_t *
fgetws(wchar_t *s, int n, FILE *f) {
    register wchar_t *p = s;
    register wchar_t *e = s + n - 1;
    register wint_t c;

    ENTER();

    if (n <= 0) {
        __set_errno(EINVAL);
        return NULL;
    }

    ORIENT(f, 0);
    flockfile(f);

    while (p < e && (c = fgetwc(f)) != WEOF && (*p++ = c) != '\n');
    *p = 0;

    funlockfile(f);

    return s;
}
