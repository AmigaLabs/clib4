/*
 * $Id: string_strncmp.c,v 1.6 2024-03-22 15:33:36 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _STRING_HEADERS_H
#include "string_headers.h"
#endif /* _STRING_HEADERS_H */

STATIC INLINE
int
__strncmp(const char *s1, const char *s2, size_t n) {
    int result = 0;

    while (n-- > 0) {
        if ((*s1) == (*s2)) {
            if ((*s1) == '\0')
                break;

            s1++;
            s2++;
        } else {
            int c1, c2;

            /* The comparison must be performed as if the
                characters were unsigned characters. */
            c1 = *(unsigned char *) s1;
            c2 = *(unsigned char *) s2;

            result = c1 - c2;
            break;
        }
    }

    return result;
}

int
strncmp(const char *s1, const char *s2, size_t n) {
    int result = 0;

    assert(s1 != NULL && s2 != NULL);
    assert((int) n >= 0);

    if (s1 == NULL || s2 == NULL) {
        __set_errno(EFAULT);
        goto out;
    }

    /* If the number of characters is 0 or negative, then this
     * function is supposed to have no effect.
     */
    if (s1 != s2 && (int) n > 0) {
        result = __strncmp(s1, s2, n);
    }

out:

    return (result);
}
