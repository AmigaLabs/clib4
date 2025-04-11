/*
 * $Id: string_mempcpy.c,v 1.0 2021-03-29 16:54:36 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _STRING_HEADERS_H
#include "string_headers.h"
#endif /* _STRING_HEADERS_H */

void *
mempcpy(void *dst0, const void *src0, size_t len0) {
    char *dst = dst0;
    const char *src = src0;
    long *aligned_dst;
    const long *aligned_src;
    int len = len0;

    ENTER();
    SHOWPOINTER(dst0);
    SHOWPOINTER(src0);
    SHOWVALUE(len0);

    /* If the size is small, or either SRC or DST is unaligned,
     then punt into the byte copy loop.  This should be rare.  */
    if (!TOO_SMALL(len) && !UNALIGNED(src, dst)) {
        aligned_dst = (long *) dst;
        aligned_src = (long *) src;

        /* Copy 4X long words at a time if possible.  */
        while (len >= BIGBLOCKSIZE) {
            *aligned_dst++ = *aligned_src++;
            *aligned_dst++ = *aligned_src++;
            *aligned_dst++ = *aligned_src++;
            *aligned_dst++ = *aligned_src++;
            len -= BIGBLOCKSIZE;
        }

        /* Copy one long word at a time if possible.  */
        while (len >= LITTLEBLOCKSIZE) {
            *aligned_dst++ = *aligned_src++;
            len -= LITTLEBLOCKSIZE;
        }

        /* Pick up any residual with a byte copier.  */
        dst = (char *) aligned_dst;
        src = (char *) aligned_src;
    }

    while (len--)
        *dst++ = *src++;

    SHOWPOINTER(dst);
    LEAVE();
    return dst;
}