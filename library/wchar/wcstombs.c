/*
 * $Id: stdlib_wcstombs.c,v 1.3 2006-01-08 12:04:26 clib2devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"

#endif /* _STDLIB_HEADERS_H */

size_t
wcstombs(char *s, const wchar_t *ws, size_t n) {
    return wcsrtombs(s, &(const wchar_t *) { ws }, n, 0);
}
