/*
 * $Id: argz_extract.c,v 1.0 2021-01-21 11:27:06 clib2devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#include <argz.h>

void 
argz_extract(char *argz, size_t argz_len, char **argv)
{
    size_t i = 0;
    size_t j = 0;
    const size_t count = argz_count(argz, argz_len);

    for (i = argz_len - 2; i > 0; i--)
    {
        if (argz[i] == '\0')
        {
            j++;
            argv[count - j] = &argz[i + 1];
        }
    }
    argv[0] = &argz[0];
    argv[count] = NULL;
}