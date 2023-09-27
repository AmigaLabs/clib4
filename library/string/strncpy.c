/*
 * $Id: string_strncpy.c,v 1.4 2006-01-08 12:04:27 clib4devs Exp $
*/

#ifndef _STRING_HEADERS_H
#include "string_headers.h"
#endif /* _STRING_HEADERS_H */

#ifndef _STDLIB_PROTOS_H
#include "stdlib_protos.h"
#endif /* _STDLIB_PROTOS_H */

char *
strncpy(char *dest, const char *src, size_t n) {
    char *result = dest;

    assert(dest != NULL && src != NULL);
    assert((int) n >= 0);

    if (dest == NULL || src == NULL) {
        __set_errno(EFAULT);
        goto out;
    }

    /* If the number of characters is 0 or negative, then this
     * function is supposed to have no effect.
     */
    if ((int) n > 0) {
        while (n-- > 0) {
            if (((*dest++) = (*src++)) == '\0') {
                /* Fill the rest with NUL bytes... */
                while (n-- > 0)
                    (*dest++) = '\0';

                break;
            }
        }
    }

out:

    return (result);
}
