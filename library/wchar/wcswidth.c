/*
 * $Id: wchar_wcswidth.c,v 1.0 2021-02-21 22:50:26 clib2devs Exp $
*/

#ifndef _WCHAR_HEADERS_H
#include "wchar_headers.h"
#endif /* _WCHAR_HEADERS_H */

int 
wcswidth(const wchar_t *pwcs, size_t n)
{
    int w, len = 0;
    if (!pwcs || n == 0)
        return 0;
    do
    {
        if ((w = wcwidth(*pwcs)) < 0)
            return -1;
        len += w;
    } while (*pwcs++ && --n > 0);
    return len;
}
