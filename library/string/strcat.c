/*
 * $Id: string_strcat.c,v 1.4 2006-01-08 12:04:26 clib2devs Exp $
*/

#ifndef _STRING_HEADERS_H
#include "string_headers.h"
#endif /* _STRING_HEADERS_H */

#ifndef _STDLIB_PROTOS_H
#include "stdlib_protos.h"
#endif /* _STDLIB_PROTOS_H */

char *
strcat(char *dest, const char *src) {
    char *result = dest;

    assert(dest != NULL && src != NULL);

    if (dest == NULL || src == NULL) {
        __set_errno(EFAULT);
        goto out;
    }

    while ((*dest) != '\0')
        dest++;

    while (((*dest++) = (*src++)) != '\0')
        DO_NOTHING;

out:

    return (result);
}
