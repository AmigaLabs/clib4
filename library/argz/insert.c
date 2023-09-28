/*
 * $Id: argz_insert.c,v 1.0 2021-01-21 10:08:32 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#include <argz.h>

error_t
argz_insert(char **argz, size_t *argz_len, char *before, const char *entry)
{
    size_t len = 0;

    if (before == NULL)
        return argz_add(argz, argz_len, entry);

    if (before < *argz || before >= *argz + *argz_len)
        return EINVAL;

    while (before != *argz && before[-1])
        before--;

    len = strlen(entry) + 1;

    if (!(*argz = (char *)realloc(*argz, *argz_len + len)))
        return ENOMEM;

    memmove(before + len, before, (size_t) (*argz + *argz_len - before));
    memcpy(before, entry, len);

    *argz_len += len;

    return 0;
}