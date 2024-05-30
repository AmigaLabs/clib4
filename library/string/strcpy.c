/*
 * $Id: string_strcpy.c,v 1.6 2024-03-22 12:04:27 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _STRING_HEADERS_H
#include "string_headers.h"
#endif /* _STRING_HEADERS_H */

char *
strcpy(char *dest, const char *src) {
    return __strcpy_ppc(dest, src);
}
