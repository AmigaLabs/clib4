/*
 * $Id: string_strpbrk.c,v 1.4 2006-01-08 12:04:27 clib4devs Exp $
*/

#ifndef _STRING_HEADERS_H
#include "string_headers.h"
#endif /* _STRING_HEADERS_H */

#ifndef _STDLIB_PROTOS_H
#include "stdlib_protos.h"
#endif /* _STDLIB_PROTOS_H */

char *
strpbrk(const char *s, const char *set) {
    char *result = NULL;
    char c;

    assert(s != NULL && set != NULL);

    if (s == NULL || set == NULL) {
        __set_errno(EFAULT);
        goto out;
    }

    while ((c = (*s)) != '\0') {
        /* Is this character found in the set? */
        if (strchr(set, c) != NULL) {
            /* If it's in there, remember where we found it. */
            result = (char *) s;
            break;
        }

        s++;
    }

out:

    return (result);
}
