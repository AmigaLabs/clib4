/*
 * $Id: unistd_execv.c,v 1.3 2006-08-02 08:00:27 clib2devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

int
execv(const char *path, char *const argv[]) {
    int result;

    result = execve(path, argv, environ);

    return (result);
}
