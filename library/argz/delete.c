/*
 * $Id: argz_delete.c,v 1.0 2021-01-21 11:25:47 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#include <argz.h>

error_t
argz_delete(char **argz, size_t *argz_len, char *entry) {
    size_t len = 0;
    char *moveto = entry;

    if (entry) {
        len = strlen(entry) + 1;
        entry += len;

        memmove(moveto, entry, (size_t)(*argz + *argz_len - entry));

        *argz_len -= len;

        if (!(*argz = (char *) realloc(*argz, *argz_len)))
            return ENOMEM;

        if (*argz_len <= 0) {
            free(*argz);
            *argz = NULL;
        }
    }
    return 0;
}