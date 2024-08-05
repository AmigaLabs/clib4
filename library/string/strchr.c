/*
 * $Id: string_strchr.c,v 1.7 2024-08-05 12:04:26 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _STRING_HEADERS_H
#include "string_headers.h"
#endif /* _STRING_HEADERS_H */

char *
strchr(const char *s, int c) {
    return __strchr_ppc(s, c);
}
