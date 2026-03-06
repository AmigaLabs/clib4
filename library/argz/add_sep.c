/*
 * $Id: argz_add_sep.c,v 1.0 2021-01-21 10:08:32 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#include <argz.h>

error_t
argz_add_sep(char **argz, size_t *argz_len, const char *str, int sep)
{
    char *str_argz = 0;
    size_t str_argz_len = 0;
    size_t last = *argz_len;
    error_t rc;

    rc = argz_create_sep(str, sep, &str_argz, &str_argz_len);
    if (rc) {
        free(str_argz);
        return rc;
    }

    *argz_len += str_argz_len;

    {
        char *tmp = (char *)realloc(*argz, *argz_len);
        if (!tmp) {
            *argz_len = last;
            free(str_argz);
            return ENOMEM;
        }
        *argz = tmp;
    }

    memcpy(*argz + last, str_argz, str_argz_len);
    free(str_argz);
    return 0;
}
