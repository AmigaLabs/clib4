/*
 * $Id: wchar_wcsrtombs.c,v 1.3 2006-01-08 12:04:27 clib2devs Exp $
*/

#ifndef _WCHAR_HEADERS_H
#include "wchar_headers.h"
#endif /* _WCHAR_HEADERS_H */
 
size_t
wcsrtombs(char *s, const wchar_t **ws, size_t n, mbstate_t *st)
{
    ENTER();

    return wcsnrtombs(s, ws, (size_t) -1, n, st);
}
