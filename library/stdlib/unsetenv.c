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
    char *name_copy = NULL;
    int result = -1;
    LONG status;
    size_t i;
    struct _clib4 *__clib4 = __CLIB4;

    __check_abort_f(__clib4);

    assert(name != NULL);

    if (name == NULL) {
        __set_errno_r(__clib4, EFAULT);
        goto out;
    }

    for (i = 0; i < strlen(name); i++) {
        if (name[i] == '=') {
            name_copy = __malloc_r(__clib4, i + 1);
            if (name_copy == NULL)
                goto out;

            memmove(name_copy, name, i);
            name_copy[i] = '\0';

            name = name_copy;
            break;
        }
    }

    status = DeleteVar((STRPTR) name, 0);
    if (status == DOSFALSE) {
        __set_errno_r(__clib4, __translate_access_io_error_to_errno(IoErr()));
        goto out;
    }

    result = 0;

out:

    if (name_copy != NULL)
        __free_r(__clib4, name_copy);

    return result;
}
