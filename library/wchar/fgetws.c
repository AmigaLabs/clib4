/*
 * $Id: wchar_fgetws.c,v 1.4 2024-07-20 12:04:27 clib4devs Exp $
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
    struct _clib4 *__clib4 = __CLIB4;

    ENTER();

    if (n <= 0) {
        __set_errno_r(__clib4, EINVAL);

        RETURN(NULL);
        return NULL;
    }

    ORIENT(f, 0);
    __flockfile_r(__clib4, f);

    while (p < e && (c = fgetwc(f)) != WEOF && (*p++ = c) != '\n');
    *p = 0;

    __funlockfile_r(__clib4, f);

    return s;
}
