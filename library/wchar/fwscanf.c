/*
 * $Id: wchar_fwscanf.c,v 1.0 2022-03-05 18:32:07 clib4devs Exp $
*/

#ifndef _WCHAR_HEADERS_H
#include "wchar_headers.h"
#endif /* _WCHAR_HEADERS_H */

int
fwscanf(FILE *stream, const wchar_t *format, ...) {
    int ret = EOF;

    ENTER();

    SHOWWSTRING(format);

    assert(format != NULL);

    if (format == NULL) {
        __set_errno(EFAULT);
        goto out;
    }

    va_list ap;
    va_start(ap, format);

    ret = vfwscanf(stream, format, ap);
    va_end(ap);

out:
    RETURN(ret);
    return ret;
}
