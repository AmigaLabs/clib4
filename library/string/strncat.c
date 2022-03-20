/*
 * $Id: string_strncat.c,v 1.4 2006-01-08 12:04:27 clib2devs Exp $
*/

#ifndef _STRING_HEADERS_H
#include "string_headers.h"
#endif /* _STRING_HEADERS_H */

#ifndef _STDLIB_PROTOS_H
#include "stdlib_protos.h"
#endif /* _STDLIB_PROTOS_H */

char *
strncat(char *dest, const char *src, size_t n) {
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
        while ((*dest) != '\0')
            dest++;

        while (n-- > 0) {
            if (((*dest++) = (*src++)) == '\0') {
                /* Back up one character so that the
                   terminating NUL is written in the
                   correct location. */
                dest--;
                break;
            }
        }

        /* Always NUL-terminate the destination string. */
        (*dest) = '\0';
    }

out:

    return (result);
}
