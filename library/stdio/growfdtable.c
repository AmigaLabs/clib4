/*
 * $Id: stdio_growfdtable.c,v 1.9 2006-01-08 12:04:24 clib4devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#ifndef _STDLIB_MEMORY_H
#include "stdlib_memory.h"
#endif /* _STDLIB_MEMORY_H */

int
__grow_fd_table(struct _clib4 *__clib4, int max_fd) {
    const int granularity = 10;
    int new_num_fd;
    int result = ERROR;

    if (max_fd == 0)
        new_num_fd = __clib4->__num_fd + granularity;
    else
        new_num_fd = max_fd;

    if (new_num_fd > __clib4->__num_fd) {
        struct fd **new_fd;
        int i;

        new_fd = __malloc_r(__clib4, sizeof(*new_fd) * new_num_fd);
        if (new_fd == NULL) {
            SHOWMSG("not enough memory for new file descriptor table");

            __set_errno(ENOMEM);
            goto out;
        }

        for (i = __clib4->__num_fd; i < new_num_fd; i++) {
            new_fd[i] = __malloc_r(__clib4, sizeof(*new_fd[i]));
            if (new_fd[i] == NULL) {
                int j;

                SHOWMSG("not enough memory for new file descriptor table entry");

                for (j = __clib4->__num_fd; j < i; j++) {
                    UnlockMem(new_fd[j], sizeof(*new_fd[j]));
                    __free_r(__clib4, new_fd[j]);
                }

                __free_r(__clib4, new_fd);

                __set_errno(ENOMEM);
                goto out;
            }

            memset(new_fd[i], 0, sizeof(*new_fd[i]));
            LockMem(new_fd[i], sizeof(*new_fd[i]));
        }

        if (__clib4->__fd != NULL) {
            for (i = 0; i < __clib4->__num_fd; i++)
                new_fd[i] = __clib4->__fd[i];

            __free_r(__clib4, __clib4->__fd);
        }

        __clib4->__fd = new_fd;
        __clib4->__num_fd = new_num_fd;
    }

    result = OK;

out:

    return (result);
}
