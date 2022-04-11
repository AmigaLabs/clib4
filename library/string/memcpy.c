/*
 * $Id: string_memcpy.c,v 1.8 2021-03-22 09:02:51 clib2devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _STRING_HEADERS_H
#include "string_headers.h"
#endif /* _STRING_HEADERS_H */

void *
memcpy(void *dst, const void *src, size_t len) {
    void *result = dst;

    assert((len == 0) || (dst != NULL && src != NULL && (int) len > 0));

    if (dst == NULL || src == NULL) {
        __set_errno(EFAULT);
        goto out;
    }

    if (len > 0 && dst != src) {
        char *to = dst;
        const char *from = src;

        /* The two memory regions may not overlap. */
        assert((to) >= (from) + len || (from) >= (to) + len);

        /* memcpy will use Exec CopyMem that is ALWAYS optimized for any platform */
        CopyMem((unsigned char *) src, (unsigned char *) dst, len);
    } else
        __set_errno(EFAULT);

    out:

    return (result);
}
