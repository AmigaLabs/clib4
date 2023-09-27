/*
 * $Id: string_strdup.c,v 1.9 2006-01-08 12:04:27 clib4devs Exp $
*/

#ifndef _STRING_HEADERS_H
#include "string_headers.h"
#endif /* _STRING_HEADERS_H */

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _STDLIB_MEMORY_H
#include "stdlib_memory.h"
#endif /* _STDLIB_MEMORY_H */

#undef strdup

__static char *
__strdup(const char *s) {
    char *result = NULL;
    size_t total_size;

    assert(s != NULL);

    if (s == NULL) {
        __set_errno(EFAULT);
        goto out;
    }

    total_size = strlen(s) + 1;

    result = malloc(total_size);
    if (result == NULL)
        goto out;

    memmove(result, s, total_size);

out:

    return (result);
}

char *
strdup(const char *s) {
    char *result;

    result = __strdup(s);

    return (result);
}
