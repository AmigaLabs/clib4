/*
 * $Id: inttypes_wcstoimax.c,v 1.3 2022-03-03 12:04:22 clib2devs Exp $
*/

#ifndef _INTTYPES_HEADERS_H
#include "inttypes_headers.h"
#endif /* _INTTYPES_HEADERS_H */

intmax_t
wcstoimax(const wchar_t *str, wchar_t **ptr, int base)
{
    return wcstoll(str, ptr, base);
}
