/*
 * $Id: stdio_tempnam.c,v 1.0 2021-02-22 17:46:25 clib4devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

/* Try to open the file specified, if it can't be opened then try
   another one.  Return nonzero if successful, otherwise zero.  */

static int
worker(char *result, const char *part1, const char *part2, char *part3, int *part4) {
    /*  Generate the filename and make sure that there isn't one called it already.  */
    struct _clib4 *__clib4 = __CLIB4;

    while (1) {
        __check_abort_f(__clib4);

        int t;

        if (!__clib4->__unix_path_semantics) {
            if (!strcmp(part1, P_tmpdir)) {
                sprintf(result, "T:%s%s.%x", part2, part3, *part4);
            } else {
                size_t len = strlen(part1);

                if (len == 0 || part1[len - 1] == ':' || part1[len - 1] == '/') {
                    sprintf(result, "%s%s%s.%x", part1, part2, part3, *part4);
                } else {
                    sprintf(result, "%s/%s%s.%x", part1, part2, part3, *part4);
                }
            }
        } else
            sprintf(result, "%s/%s%s.%x", part1, part2, part3, *part4);
        (*part4)++;
        t = open(result, O_RDONLY, 0);
        if (t == -1) {
            if (errno == ENOSYS) {
                result[0] = '\0';
                return 0;
            }
            break;
        }
        close(t);
    }
    return 1;
}

char *
tempnam(const char *dir, const char *pfx) {
    ENTER();
    struct _clib4 *__clib4 = __CLIB4;

    __check_abort_f(__clib4);

    char *filename;
    int length;
    const char *prefix = (pfx) ? pfx : "";

    if (dir == NULL && (dir = getenv("TMPDIR")) == NULL)
        dir = P_tmpdir;

    /* two 8 digit numbers + . / */
    length = strlen(dir) + strlen(prefix) + (4 * sizeof(int)) + 2 + 1;

    if (length >= PATH_MAX) {
        __set_errno_r(__clib4, ENAMETOOLONG);
        return 0;
    }

    filename = __malloc_r(__clib4, length);
    if (filename) {
        int r = rand();
        char string[7] = {0};

        snprintf(string, 7, "%lX", r);
        if (!worker(filename, dir, prefix, string, &__clib4->inc))
            return NULL;
    }

    RETURN(filename);
    return filename;
}
