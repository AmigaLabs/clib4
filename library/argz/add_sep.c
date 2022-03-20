/*
 * $Id: argz_add_sep.c,v 1.0 2021-01-21 10:08:32 clib2devs Exp $
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

    argz_create_sep(str, sep, &str_argz, &str_argz_len);

    *argz_len += str_argz_len;

    if (!(*argz = (char *)realloc(*argz, *argz_len)))
        return ENOMEM;

    memcpy(*argz + last, str_argz, str_argz_len);
    return 0;
}
