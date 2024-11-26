/*
 * $Id: glob_globfree.c,v 1.0 2021-01-22 12:28:38 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#include <glob.h>

void
globfree(glob_t *pglob) {
    int i;
    char **pp;

    ENTER();

    SHOWPOINTER(pglob);

    if (pglob->gl_pathv != NULL) {
        pp = pglob->gl_pathv + pglob->gl_offs;
        for (i = pglob->gl_pathc; i--; ++pp)
            if (*pp)
                free(*pp);
        free(pglob->gl_pathv);
        pglob->gl_pathv = NULL;
        pglob->gl_pathc = 0;
    }

    LEAVE();
}