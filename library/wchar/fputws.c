/*
 * $Id: wchar_fputws.c,v 1.3 2006-01-08 12:04:27 clib4devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#ifndef _WCHAR_HEADERS_H
#include "wchar_headers.h"
#endif /* _WCHAR_HEADERS_H */

int
fputws(const wchar_t *ws, FILE *fp) {
    char buf[BUFSIZ];
    int nwritten = 0;
    struct _clib4 *__clib4 = __CLIB4;

    ENTER();

    if (fp == NULL || ws == NULL) {
        __set_errno_r(__clib4, EFAULT);
        RETURN(EOF);
        return EOF;
    }

    __check_abort_f(__clib4);

    __flockfile_r(__clib4, fp);

    fwide(fp, 1);

    while (ws && (nwritten = wcsrtombs((void *) buf, (void *) &ws, sizeof buf, 0)) + 1 > 1) {
        if (fwrite(buf, nwritten, 1, fp) < nwritten) {
            nwritten = EOF;
            goto out;
        }
    }

out:

    __funlockfile_r(__clib4, fp);

    RETURN(nwritten);
    return (nwritten);
}
