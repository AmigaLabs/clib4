/*
 * $Id: inttypes_wcstoumax.c,v 1.3 2022-03-03 20:02:22 clib4devs Exp $
*/

#ifndef _INTTYPES_HEADERS_H
#include "inttypes_headers.h"
#endif /* _INTTYPES_HEADERS_H */

uintmax_t
wcstoumax(const wchar_t *str, wchar_t **ptr, int base)
{
    return wcstoull(str, ptr, base);
}
