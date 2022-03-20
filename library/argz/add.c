/*
 * $Id: argz_add.c,v 1.0 2021-01-21 10:08:32 clib2devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#include <argz.h>

error_t
argz_add(char **argz, size_t *argz_len, const char *str)
{
    size_t len_to_add = 0;
    size_t last = *argz_len;

    if (str == NULL)
        return 0;

    len_to_add = strlen(str) + 1;
    *argz_len += len_to_add;

    if (!(*argz = (char *)realloc(*argz, *argz_len)))
        return ENOMEM;

    memcpy(*argz + last, str, len_to_add);
    return 0;
}
