/*
 * $Id: argz_stringify.c,v 1.0 2021-01-21 11:39:35 clib2devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#include <argz.h>

void
argz_stringify(char *argz, size_t argz_len, int sep)
{
    size_t i;

    /* len includes trailing \0, which we don't want to replace. */
    for (i = 0; i < argz_len - 1; i++)
    {
        if (argz[i] == '\0')
            argz[i] = sep;
    }
}