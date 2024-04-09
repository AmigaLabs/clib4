/*
 * $Id: stdio_init_exit.c,v 1.34 2024-04-09 12:04:24 clib4devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

#ifndef _STDLIB_MEMORY_H
#include "stdlib_memory.h"
#endif /* _STDLIB_MEMORY_H */

void
__close_all_files(struct _clib4 *__clib4) {
    int i;

    ENTER();

    __stdio_lock(__clib4);

    if (__clib4->__num_iob > 0) {
        for (i = 0; i < __clib4->__num_iob; i++) {
            if (FLAG_IS_SET(__clib4->__iob[i]->iob_Flags, IOBF_IN_USE))
                fclose((FILE *) __clib4->__iob[i]);
        }

        __clib4->__num_iob = 0;
    }

    if (__clib4->__num_fd > 0) {
        for (i = 0; i < __clib4->__num_fd; i++) {
            if (FLAG_IS_SET(__clib4->__fd[i]->fd_Flags, FDF_IN_USE))
                close(i);
            UnlockMem(__clib4->__fd[i], sizeof(*__clib4->__fd[i]));
        }
        __clib4->__num_fd = 0;
    }

    __stdio_unlock(__clib4);

    LEAVE();
}

void _stdio_exit(void) {
    ENTER();
    struct _clib4 *__clib4 = __CLIB4;

    __close_all_files(__clib4);

    __stdio_lock_exit(__clib4);

    LEAVE();
}

void _stdio_init(void) {
    const int num_standard_files = (STDERR_FILENO - STDIN_FILENO + 1);
    struct _clib4 *__clib4 = __CLIB4;

    ENTER();

    if (__stdio_lock_init(__clib4) < 0)
        goto out;

    if (__grow_iob_table(__clib4, num_standard_files) < 0)
        goto out;

    if (__grow_fd_table(__clib4, num_standard_files) < 0)
        goto out;

out:

    LEAVE();
}
