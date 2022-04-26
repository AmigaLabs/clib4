/*
 * $Id: wchar_wscoll.c,v 1.4 2021-02-04 00:29:27 clib2devs Exp $
*/

#ifndef _WCHAR_HEADERS_H
#include "wchar_headers.h"
#endif /* _WCHAR_HEADERS_H */

int
wscoll(const wchar_t *s1, const wchar_t *s2) {
    return wcscmp(s1, s2);
}
