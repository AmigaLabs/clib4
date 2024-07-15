/*
 * $Id: wchar_c32rtomb.c,v 1.0 2022-08-04 19:11:16 clib4devs Exp $
*/

#ifndef _WCHAR_HEADERS_H
#include "wchar_headers.h"
#endif /* _WCHAR_HEADERS_H */

#include <uchar.h>

size_t
c32rtomb(char *s, char32_t c32, mbstate_t *ps) {
    return wcrtomb(s, c32, ps);
}
