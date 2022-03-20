/*
 * $Id: argz_count.c,v 1.0 2021-01-21 11:24:12 clib2devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#include <argz.h>

size_t
argz_count(const char *argz, size_t argz_len)
{
    unsigned int i;
    size_t count = 0;

    for (i = 0; i < argz_len; i++)
    {
        if (argz[i] == '\0')
            count++;
    }
    return count;
}