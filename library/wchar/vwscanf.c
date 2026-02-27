/*
 * $Id: wchar_vwscanf.c,v 1.0 2022-08-04 19:47:27 clib4devs Exp $
*/

#ifndef _WCHAR_HEADERS_H
#include "wchar_headers.h"
#endif /* _WCHAR_HEADERS_H */

#include "stdio_headers.h"

int
vwscanf(const wchar_t *fmt, va_list ap) {
    int result = EOF;
    struct _clib4 *__clib4 = __CLIB4;

    ENTER();

    assert(fmt != NULL);

    if (fmt == NULL) {
        SHOWMSG("invalid format parameter");

        __set_errno_r(__clib4, EFAULT);
        goto out;
    }

    result = vfwscanf(__stdin_r(__clib4), fmt, ap);

out:

    RETURN(result);
    return result;
}