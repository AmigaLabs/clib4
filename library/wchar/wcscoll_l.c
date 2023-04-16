/*
 * $Id: wchar_wscoll_l.c,v 1.0 2022-08-04 12:04:27 clib2devs Exp $
*/

#ifndef _WCHAR_HEADERS_H
#include "wchar_headers.h"
#endif /* _WCHAR_HEADERS_H */

int
wcscoll_l(const wchar_t *ws1, const wchar_t *ws2, locale_t locale) {
    (void) locale;

    return wcscmp(ws1, ws2);
}
