/*
 * $Id: envz_envz_remove.c,v 1.0 2023-04-16 12:04:22 clib4devs Exp $
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
envz_remove(char **envz, size_t *envz_len, const char *name) {
    char *entry = NULL;
    int len = 0;
    entry = envz_entry(*envz, *envz_len, name);

    if (entry) {
        len = strlen(entry) + 1;

        /* Not the last entry. */
        if (*envz + *envz_len != entry + len - 1) {
            memmove(entry, entry + len, *envz + *envz_len - entry - len);
        }

        *envz = (char *) realloc(*envz, *envz_len - len);
        *envz_len -= len;
    }
}
