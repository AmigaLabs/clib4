/*
 * $Id: string_strchr.c,v 1.6 2022-03-22 12:04:26 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _STRING_HEADERS_H
#include "string_headers.h"
#endif /* _STRING_HEADERS_H */

char *
strchr(const char *s, int c) {
    const unsigned char *us = (const unsigned char *) s;
    char *result = NULL;
    unsigned char us_c;
    unsigned char find_this = (c & 0xff);

    assert(s != NULL);

    if (us == NULL) {
        __set_errno(EFAULT);
        goto out;
    }

    while (TRUE) {
        us_c = (*us);
        if (us_c == find_this) {
            result = (char *) us;
            break;
        }

        if (us_c == '\0')
            break;

        us++;
    }

out:

    return (result);
}
