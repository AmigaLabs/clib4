/*
 * $Id: stdlib_unsetenv.c,v 1.11 2024-07-22 16:32:49 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#ifndef _STDLIB_MEMORY_H
#include "stdlib_memory.h"
#endif /* _STDLIB_MEMORY_H */

int
unsetenv(const char *name) {
    int result = -1;
    struct _clib4 *__clib4 = __CLIB4;

    assert(name != NULL);

    if (name == NULL) {
        __set_errno_r(__clib4, EFAULT);
        return result;
    }

    register char **P;
    int offset;
    char ***p_environ = &environ;

    MutexObtain(__clib4->__environment_lock);

    while (getenv_r(__clib4, name, &offset)) /* if set multiple times */
        for (P = &(*p_environ)[offset];; ++P)
            if (!(*P = *(P + 1)))
                break;

    MutexRelease(__clib4->__environment_lock);

    return result;
}
