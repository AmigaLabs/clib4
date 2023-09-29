/*
 * $Id: string_strstr.c,v 1.4 2006-01-08 12:04:27 clib4devs Exp $
*/

#ifndef _STRING_HEADERS_H
#include "string_headers.h"
#endif /* _STRING_HEADERS_H */

#ifndef _STDLIB_PROTOS_H
#include "stdlib_protos.h"
#endif /* _STDLIB_PROTOS_H */

char *
strstr(const char *src, const char *sub) {
    char *result = NULL;

    assert(src != NULL && sub != NULL);

    if (src == NULL || sub == NULL) {
        __set_errno(EFAULT);
        goto out;
    }

    /* If 'sub' points to a string with zero length, then
     * we must return 'src'.
     */
    if ((*sub) == '\0') {
        result = (char *) src;
    } else {
        size_t i;
        char c;

        /* Try to find 'sub' in 'src'. Watch how we avoid calling
         * strlen() even once...
         */
        while ((*src) != '\0') {
            for (i = 0;; i++) {
                c = sub[i];

                /* End of substring? We got a match... */
                if (c == '\0') {
                    result = (char *) src;
                    goto out;
                }

                if (c != src[i])
                    break;
            }

            src++;
        }
    }

out:

    return (result);
}
