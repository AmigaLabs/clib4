/*
 * $Id: unistd_execl.c,v 1.3 2006-08-02 08:00:27 clib4devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

int
execl(const char *path, const char *arg0, ...) {
    char **argv = NULL;
    int result = -1;
    size_t argc = 0;
    va_list args;
    struct _clib4 *__clib4 = __CLIB4;

    /* If there are any, count the number of arguments supplied */
    if (arg0 != NULL) {
        argc = 1;

        va_start(args, arg0);

        while (va_arg(args,const char *) != NULL)
        argc++;

        va_end(args);
    }

    /* Allocate enough memory for the entire argument list
       plus the terminating NULL */
    argv = (char **) __malloc_r(__clib4, sizeof(*argv) * (argc + 1));
    if (argv == NULL) {
        __set_errno(ENOMEM);
        goto out;
    }

    if (argc > 0) {
        size_t i;

        argv[0] = (char *) arg0;

        va_start(args, arg0);

        for (i = 1; i < argc; i++)
            argv[i] = va_arg(args,
        char *);

        va_end(args);
    }

    argv[argc] = NULL;

    result = execve(path, (char *const *) argv, __clib4->__environment);

out:

    if (argv != NULL)
        __free_r(__clib4, argv);

    return (result);
}
