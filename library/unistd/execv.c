/*
 * $Id: unistd_execv.c,v 1.3 2006-08-02 08:00:27 clib4devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

int
execv(const char *path, char *const argv[]) {
    int result;
    struct _clib4 *__clib4 = __CLIB4;

    result = execve(path, argv, __clib4->__environment);

    return (result);
}
