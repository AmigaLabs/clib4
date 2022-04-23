/*
 * $Id: wchar_wscanf.c,v 1.3 2006-01-08 12:04:27 clib2devs Exp $
*/

#ifndef _WCHAR_HEADERS_H
#include "wchar_headers.h"
#endif /* _WCHAR_HEADERS_H */

int
wscanf(const wchar_t *format, ...) {
    int ret;
    va_list ap;

    ENTER();

    va_start(ap, format);
    ret = vfwscanf(stdin, format, ap);
    va_end(ap);

    RETURN(ret);
    return ret;
}
