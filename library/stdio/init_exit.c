/*
 * $Id: stdio_init_exit.c,v 1.33 2006-01-08 12:04:24 clib4devs Exp $
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

#ifndef _STDLIB_CONSTRUCTOR_H
#include "stdlib_constructor.h"
#endif /* _STDLIB_CONSTRUCTOR_H */

void
__close_all_files(struct _clib4 *__clib4) {
    int i;

    ENTER();

    __stdio_lock(__clib4);

    if (__clib4->__num_iob > 0) {
        for (i = 0; i < __clib4->__num_iob; i++) {
            if (FLAG_IS_SET(__clib4->__iob[i]->iob_Flags, IOBF_IN_USE)) {
                D(("Close __iob %ld\n", i));
                fclose((FILE *) __clib4->__iob[i]);
                __clib4->__num_iob--;
            }
        }
    }

    if (__clib4->__num_fd > 0) {
        for (i = 0; i < __clib4->__num_fd; i++) {
            /* If file is set as in use close it only if it isn't marked as FDF_NO_CLOSE */
            if (FLAG_IS_SET(__clib4->__fd[i]->fd_Flags, FDF_IN_USE) && FLAG_IS_CLEAR(__clib4->__fd[i]->fd_Flags, FDF_NO_CLOSE)) {
                D(("Close __fd %ld\n", i));
                close(i);
                UnlockMem(__clib4->__fd[i], sizeof(*__clib4->__fd[i]));
                __clib4->__num_fd--;
            }
            else {
                D(("Can't close __fd %d FDF_IN_USE=%d FDF_NO_CLOSE=%d \n", i, FLAG_IS_SET(__clib4->__fd[i]->fd_Flags, FDF_IN_USE), FLAG_IS_SET(__clib4->__fd[i]->fd_Flags, FDF_NO_CLOSE)));
            }
        }
    }

    __stdio_unlock(__clib4);

    LEAVE();
}

STDIO_DESTRUCTOR(stdio_exit) {
    ENTER();
    struct _clib4 *__clib4 = __CLIB4;

    __close_all_files(__clib4);

    __stdio_lock_exit(__clib4);

    LEAVE();
}

STDIO_CONSTRUCTOR(stdio_init) {
    const int num_standard_files = (STDERR_FILENO - STDIN_FILENO + 1);
    BOOL success = FALSE;
    struct _clib4 *__clib4 = __CLIB4;

    ENTER();

    if (__stdio_lock_init(__clib4) < 0)
        goto out;

    if (__grow_iob_table(__clib4, num_standard_files) < 0)
        goto out;

    if (__grow_fd_table(__clib4, num_standard_files) < 0)
        goto out;

    success = TRUE;

out:

    SHOWVALUE(success);
    LEAVE();

    if (success)
        CONSTRUCTOR_SUCCEED();
    else
        CONSTRUCTOR_FAIL();
}
