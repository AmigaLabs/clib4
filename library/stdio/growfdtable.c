/*
 * $Id: stdio_growfdtable.c,v 1.9 2006-01-08 12:04:24 clib2devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#ifndef _STDLIB_MEMORY_H
#include "stdlib_memory.h"
#endif /* _STDLIB_MEMORY_H */

int
__grow_fd_table(int max_fd) {
    const int granularity = 10;
    int new_num_fd;
    int result = ERROR;

    if (max_fd == 0)
        new_num_fd = __CLIB2->__num_fd + granularity;
    else
        new_num_fd = max_fd;

    if (new_num_fd > __CLIB2->__num_fd) {
        struct fd **new_fd;
        int i;

        new_fd = malloc(sizeof(*new_fd) * new_num_fd);
        if (new_fd == NULL) {
            SHOWMSG("not enough memory for new file descriptor table");

            __set_errno(ENOMEM);
            goto out;
        }

        for (i = __CLIB2->__num_fd; i < new_num_fd; i++) {
            new_fd[i] = malloc(sizeof(*new_fd[i]));
            if (new_fd[i] == NULL) {
                int j;

                SHOWMSG("not enough memory for new file descriptor table entry");

                for (j = __CLIB2->__num_fd; j < i; j++)
                    free(new_fd[j]);

                free(new_fd);

                __set_errno(ENOMEM);
                goto out;
            }

            memset(new_fd[i], 0, sizeof(*new_fd[i]));
        }

        if (__CLIB2->__fd != NULL) {
            for (i = 0; i < __CLIB2->__num_fd; i++)
                new_fd[i] = __CLIB2->__fd[i];

            free(__CLIB2->__fd);
        }

        __CLIB2->__fd = new_fd;
        __CLIB2->__num_fd = new_num_fd;
    }

    result = OK;

out:

    return (result);
}
