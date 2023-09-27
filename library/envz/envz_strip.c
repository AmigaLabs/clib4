/*
 * $Id: envz_envz_strip.c,v 1.0 2023-04-16 12:04:22 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _STRING_HEADERS_H
#include "string_headers.h"
#endif /* _STRING_HEADERS_H */

#include <argz.h>
#include <envz.h>

void
envz_strip(char **envz, size_t *envz_len) {
    char *entry = 0;
    int len = 0;
    int null_found = 0;

    while ((entry = argz_next(*envz, *envz_len, entry))) {
        if (!strchr(entry, '=')) {
            null_found = 1;
            len = strlen(entry) + 1;
            /* Make sure this is not the last entry in envz. If it is, it
             will be chopped off by the realloc anyway.*/
            if (*envz + *envz_len != entry + len - 1) {
                memmove(entry, entry + len, *envz + *envz_len - entry - len);
            }
            *envz_len -= len;
        }
    }
    if (null_found) {
        *envz = (char *) realloc(*envz, *envz_len);
    }
}
