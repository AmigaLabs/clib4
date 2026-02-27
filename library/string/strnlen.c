/*
 * $Id: string_strnlen.c,v 1.5 2022-08-11 12:04:27 clib4devs Exp $
*/

#ifndef _STRING_HEADERS_H
#include "string_headers.h"
#endif /* _STRING_HEADERS_H */

#ifndef _STDLIB_PROTOS_H
#include "stdlib_protos.h"
#endif /* _STDLIB_PROTOS_H */

size_t
strnlen(const char *s, size_t maxlen) {
    const char *start = s;
    size_t result = 0;

    assert(s != NULL);

    if (s == NULL) {
        __set_errno(EFAULT);
        goto out;
    }

    while (maxlen-- > 0 && *s)
        s++;

    result = s - start;

out:

    return result;
}
