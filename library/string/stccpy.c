/*
 * $Id: string_stccpy.c,v 1.0 2022-03-30 17:43:27 clib4devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

size_t
stccpy(char *dest, const char *src, size_t n) {
    char *ptr = dest;

    ENTER();
    SHOWSTRING(dest);
    SHOWSTRING(src);
    SHOWVALUE(n);

    while (n > 1 && *src) {
        *ptr = *src;
        ptr++;
        src++;
        n--;
    }

    *ptr++ = '\0';

    LEAVE();
    return (ptr - dest);
}