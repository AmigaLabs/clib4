/*
 * $Id: string_strtok_r.c,v 1.4 2006-01-08 12:04:27 clib4devs Exp $
*/

#ifndef _STRING_HEADERS_H
#include "string_headers.h"
#endif /* _STRING_HEADERS_H */

#ifndef _STDLIB_PROTOS_H
#include "stdlib_protos.h"
#endif /* _STDLIB_PROTOS_H */

char *
strtok_r(char *str, const char *separator_set, char **state_ptr) {
    char *result = NULL;
    char *last = NULL;
    size_t size;

    assert(separator_set != NULL && state_ptr != NULL);

    if (separator_set == NULL || state_ptr == NULL) {
        __set_errno(EFAULT);
        goto out;
    }

    last = (*state_ptr);

    /* Did we get called before? Restart at the last valid position. */
    if (str == NULL) {
        str = last;

        /* However, we may have hit the end of the
           string already. */
        if (str == NULL)
            goto out;
    }

    last = NULL;

    /* Skip the characters which count as
       separators. */
    str += strspn(str, separator_set);
    if ((*str) == '\0')
        goto out;

    /* Count the number of characters which aren't
       separators. */
    size = strcspn(str, separator_set);
    if (size == 0)
        goto out;

    /* This is where the search can resume later. */
    last = &str[size];

    /* If we didn't hit the end of the string already,
       skip the separator. */
    if ((*last) != '\0')
        last++;

    /* This is the token we found; make sure that
       it looks like a valid string. */
    str[size] = '\0';

    result = str;

out:

    if (state_ptr != NULL)
        (*state_ptr) = last;

    return (result);
}
