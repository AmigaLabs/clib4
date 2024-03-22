/*
 * $Id: string_strrchr.c,v 1.5 2024-03-22 12:04:27 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _STRING_HEADERS_H
#include "string_headers.h"
#endif /* _STRING_HEADERS_H */

static inline
char *
__strrchr(const char *s, int c) {
    const unsigned char *us = (const unsigned char *) s;
    char *result = NULL;
    unsigned char us_c;
    unsigned char find_this = (c & 0xff);

    /* This is technically the opposite of strchr(), but the
     * implementation is very similar. However, instead of
     * stopping on the first location that holds the desired
     * value we just remember where we found it. When the
     * end of the string finally arrives we will thus be
     * able to return a pointer to the last occurrence of
     * the character.
     */
    while (TRUE) {
        us_c = (*us);
        if (us_c == find_this)
            result = (char *) us;

        if (us_c == '\0')
            break;

        us++;
    }

    return result;
}

char *
strrchr(const char *s, int c) {
    const unsigned char *us = (const unsigned char *) s;
    char *result = NULL;

    assert(s != NULL);

    if (us == NULL) {
        __set_errno(EFAULT);
        goto out;
    }

    result = __strrchr(s, c);

out:

    return (result);
}
