/*
 * $Id: argz_next.c,v 1.0 2021-01-21 11:33:52 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#include <argz.h>

char *
argz_next(char *argz, size_t argz_len, const char *entry)
{
    if (entry)
    {
        while (*entry != '\0')
            entry++;
        entry++;

        if (entry >= argz + argz_len)
            return NULL;
        else
            return (char *)entry;
    }
    else
    {
        if (argz_len > 0)
            return (char *)argz;
        else
            return NULL;
    }
}