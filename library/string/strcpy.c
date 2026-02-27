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
strcpy(char *dst0, const char *src0) {
    char *dst = dst0;
    const char *src = src0;
    long *aligned_dst;
    const long *aligned_src;

    if (((unsigned long)src & 3) == ((unsigned long)dst & 3)) {
        /* align src and dst */
        while ((unsigned long)dst & 3)
        {
            if (!(*dst++ = *src++)) return dst0;
        }
    }

    /* If SRC or DEST is unaligned, then copy bytes.  */
    if (!UNALIGNED (src, dst)) {
        aligned_dst = (long*)dst;
        aligned_src = (long*)src;

        /* SRC and DEST are both "long int" aligned, try to do "long int"
           sized copies.  */
        while (!DETECTNULL(*aligned_src)) {
            *aligned_dst++ = *aligned_src++;
        }

        dst = (char*)aligned_dst;
        src = (char*)aligned_src;
    }

    while (*dst++ = *src++)
        ;
    return dst0;
}
