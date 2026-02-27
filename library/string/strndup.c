/*
 * $Id: string_strndup.c,v 1.0 2021-02-23 10:15:27 clib4devs Exp $
*/

#ifndef _STRING_HEADERS_H
#include "string_headers.h"
#endif /* _STRING_HEADERS_H */

#ifndef _STDLIB_HEADERS_H

#include "stdlib_headers.h"

#endif /* _STDLIB_HEADERS_H */

char *
strndup(const char *str, size_t max) {
    if (str == NULL) {
        __set_errno(EFAULT);
        return NULL;
    }

    size_t len = strnlen(str, max);
    char *res = malloc(len + 1);
    if (res) {
        memcpy(res, str, len);
        res[len] = '\0';
    }
    return res;
}
