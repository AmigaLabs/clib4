/*
 * $Id: stdlib_secure_getenv.c,v 1.0 2020-01-13 16:12:45 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#include <unistd.h>

char *
secure_getenv(const char *name) {
    ENTER();
    SHOWSTRING(name);

    if (geteuid() != getuid() || getegid() != getgid()) {
        RETURN(NULL);
        return NULL;
    }
    char *env = getenv(name);

    RETURN(env);
    return env;
}

