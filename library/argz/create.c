/*
 * $Id: argz_create.c,v 1.0 2021-01-21 10:08:32 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#include <argz.h>

error_t
argz_create(char *const argv[], char **argz, size_t *argz_len) {
    int argc = 0;
    int i = 0;
    size_t len = 0;
    char *iter;

    *argz_len = 0;

    if (*argv == NULL) {
        *argz = NULL;
        return 0;
    }

    while (argv[argc]) {
        *argz_len += (strlen(argv[argc]) + 1);
        argc++;
    }

    /* There are argc strings to copy into argz. */
    if (!(*argz = (char *) malloc(*argz_len)))
        return ENOMEM;

    iter = *argz;
    for (i = 0; i < argc; i++) {
        len = strlen(argv[i]) + 1;
        memcpy(iter, argv[i], len);
        iter += len;
    }
    return 0;
}