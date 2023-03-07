/*
 * $Id: wchar_vwscanf.c,v 1.0 2022-08-04 19:47:27 clib2devs Exp $
*/

#ifndef _WCHAR_HEADERS_H
#include "wchar_headers.h"
#endif /* _WCHAR_HEADERS_H */

int
vwscanf(const wchar_t *fmt, va_list ap) {
    int result = EOF;

    ENTER();

    assert(fmt != NULL);

    if (fmt == NULL) {
        SHOWMSG("invalid format parameter");

        __set_errno(EFAULT);
        goto out;
    }

    result = vfwscanf(stdin, fmt, ap);

out:

    RETURN(result);
    return result;
}