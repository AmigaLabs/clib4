/*
 * $Id: wchar_vwprintf.c,v 1.1 2022-03-05 19:47:27 clib2devs Exp $
*/

#ifndef _WCHAR_HEADERS_H
#include "wchar_headers.h"
#endif /* _WCHAR_HEADERS_H */

int
vwprintf(const wchar_t *format, va_list arg)
{
    ENTER();

    return vfwprintf(stdout, format, arg);
}
