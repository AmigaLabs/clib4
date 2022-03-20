/*
 * $Id: string_stpcpy.c,v 1.0 2021-01-21 17:43:27 clib2devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

#define HIGHS(x) ((x) - ((x)*0 - 1) / 255 * 128)
#define LOWS(x) (((x)*0 - 1) / 255)
#define has_zero(x) ((x)-(LOWS(x) & ~(x)&HIGHS(x)))

/* Copy SRC to DEST, returning the address of the terminating '\0' in DEST.  */
char *
stpcpy(char *dest, const char *src)
{
    size_t *wd;
    const size_t *ws;

    if ((uintptr_t)src % sizeof(size_t) != (uintptr_t)dest % sizeof(size_t))
        goto bytewise;

    for (; (uintptr_t)src % sizeof(size_t); src++, dest++)
        if (!(*dest = *src))
            return dest;

    for (wd = (size_t *)dest, ws = (const size_t *)src; !has_zero(*ws); wd++, ws++, *wd = *ws)
        ;

    dest = (char *)wd;
    src = (const char *)ws;

bytewise:
    for (; (*dest = *src); dest++, src++)
        ;

    return dest;
}