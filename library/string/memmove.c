/*
 * $Id: string_memmove.c,v 1.9 2003-09-22 09:02:51 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _STRING_HEADERS_H
#include "string_headers.h"
#endif /* _STRING_HEADERS_H */

void *
memmove(void *dest, const void *src, size_t len) {
    void *result = dest;

    assert((len == 0) || (dest != NULL && src != NULL && (int) len > 0));

    if (dest == NULL || src == NULL) {
        __set_errno(EFAULT);
        goto out;
    }

    if (len > 0 && dest != src) {
        char *to = dest;
        const char *from = src;

        DECLARE_UTILITYBASE();
        /* memmove will use Exec MoveMem that is ALWAYS optimized for any platform */
        MoveMem((APTR) src, dest, len);
    } else
        __set_errno(EFAULT);

out:
    return (result);
}
