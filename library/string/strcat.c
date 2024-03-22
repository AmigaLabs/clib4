/*
 * $Id: string_strcat.c,v 1.5 2024-03-22 12:04:26 clib4devs Exp $
*/

#ifndef _STRING_HEADERS_H
#include "string_headers.h"
#endif /* _STRING_HEADERS_H */

#ifndef _STDLIB_PROTOS_H
#include "stdlib_protos.h"
#endif /* _STDLIB_PROTOS_H */

char *
strcat(char *dest, const char *src) {
    return strcpy(dest + strlen (dest), src);
}
