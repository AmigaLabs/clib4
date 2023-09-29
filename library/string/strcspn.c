/*
 * $Id: string_strcspn.c,v 1.4 2006-01-08 12:04:27 clib4devs Exp $
*/

#ifndef _STRING_HEADERS_H
#include "string_headers.h"
#endif /* _STRING_HEADERS_H */

#ifndef _STDLIB_PROTOS_H
#include "stdlib_protos.h"
#endif /* _STDLIB_PROTOS_H */

size_t
strcspn(const char *s, const char *set) {
    const char *start = s;
    size_t result = 0;
    char c;

    assert(s != NULL && set != NULL);

    if (s == NULL || set == NULL) {
        __set_errno(EFAULT);
        goto out;
    }

    while ((c = (*s)) != '\0') {
        /* Is this character found in the set? */
        if (strchr(set, c) != NULL)
            break; /* Stop if it's in there. */

        s++;
    }

    result = (size_t)(s - start);

out:

    return (result);
}
