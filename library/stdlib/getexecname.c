/*
 * $Id: stdlib_getexecname.c,v 1.0 2020-01-13 13:28:26 clib2devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

const char *
getexecname(void)
{
    char pathBuffer[PATH_MAX] = {0};
    if (GetCliProgramName(pathBuffer, PATH_MAX - 1))
    {
        char *ret = calloc(1, strlen(pathBuffer) + 1);
        return ret;
    }
    else
    {
        return "?";
    }
}
