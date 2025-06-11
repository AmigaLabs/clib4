/*
 * $Id: stdlib_getexecname.c,v 1.0 2020-01-13 13:28:26 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

const char *
getexecname(void) {
    ENTER();

    char pathBuffer[PATH_MAX] = {0};
    if (GetCliProgramName(pathBuffer, PATH_MAX - 1)) {
        char *ret = calloc(1, strlen(pathBuffer) + 1);
        strncpy(ret, pathBuffer, PATH_MAX - 1);
        RETURN(ret);
        return ret;
    } else {
        RETURN(NULL);
        return NULL;
    }
}
