/*
 * $Id: unistd_execv.c,v 1.3 2006-08-02 08:00:27 clib2devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

int
execv(const char *path, char *const argv[]) {
    int result;
    struct _clib2 *__clib2 = __CLIB2;

    result = execve(path, argv, __clib2->__environment);

    return (result);
}
