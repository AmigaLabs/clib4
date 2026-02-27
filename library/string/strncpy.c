/*
 * $Id: string_strncpy.c,v 1.4 2025-04-01 12:04:27 clib4devs Exp $
*/

#ifndef _STRING_HEADERS_H
#include "string_headers.h"
#endif /* _STRING_HEADERS_H */

#ifndef _STDLIB_PROTOS_H
#include "stdlib_protos.h"
#endif /* _STDLIB_PROTOS_H */

char *
strncpy(char *dst0, const char *src0, size_t count) {
    char *dst = dst0;
    const char *src = src0;
    long *aligned_dst;
    const long *aligned_src;

    if (count > 3 && (((unsigned long)src & 3) == ((unsigned long)dst & 3))) {
        /* align src and dst */
        while ((unsigned long)dst & 3) {
            count--;
            if (!(*dst++ = *src++)) break;
        }
    }

    /* If SRC and DEST is aligned and count large enough, then copy words.  */
    if (!UNALIGNED (src, dst) && !TOO_SMALL (count)) {
        aligned_dst = (long*)dst;
        aligned_src = (long*)src;

        /* SRC and DEST are both "long int" aligned, try to do "long int"
       sized copies.  */
        while (count >= sizeof (long int) && !DETECTNULL(*aligned_src)) {
            count -= sizeof (long int);
            *aligned_dst++ = *aligned_src++;
        }

        dst = (char*)aligned_dst;
        src = (char*)aligned_src;
    }

    while (count > 0) {
        --count;
        if ((*dst++ = *src++) == '\0')
            break;
    }

    while (count-- > 0)
        *dst++ = '\0';

    return dst0;
}
