/*
 * $Id: string_strnlen.c,v 1.4 2006-01-08 12:04:27 clib2devs Exp $
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

    while ((maxlen != 0) && ((*s) != '\0')) {
        s++;
        maxlen--;
    }

    result = (size_t)(s - start);

out:

    return (result);
}
