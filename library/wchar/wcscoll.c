/*
 * $Id: wchar_wscoll.c,v 1.3 2006-01-08 12:04:27 clib2devs Exp $
*/

#ifndef _WCHAR_HEADERS_H
#include "wchar_headers.h"
#endif /* _WCHAR_HEADERS_H */

int
wcscoll(const wchar_t *ws1, const wchar_t *ws2) {
    return wcscmp(ws1, ws2);
}
