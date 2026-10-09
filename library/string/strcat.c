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
#if defined(__arm__)
    /* ARM has no PowerPC routine behind __strcat_ppc: append in C. */
    char *d = dest;

    while (*d != '\0')
        d++;
    while ((*d++ = *src++) != '\0')
        ;
    return dest;
#else
    return __strcat_ppc(dest, src);
#endif
}
