/*
 * $Id: argz_append.c,v 1.0 2021-01-21 10:48:32 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#include <argz.h>

error_t
argz_append(char **argz, size_t *argz_len, const char *buf, size_t buf_len)
{
    size_t last = *argz_len;

    *argz_len += buf_len;

    if (!(*argz = (char *)realloc(*argz, *argz_len)))
        return ENOMEM;

    memcpy(*argz + last, buf, buf_len);
    return 0;
}
