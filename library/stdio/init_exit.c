/*
 * $Id: stdio_init_exit.c,v 1.33 2006-01-08 12:04:24 clib2devs Exp $
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
__close_all_files(void) {
    int i;
    struct _clib2 *__clib2 = __CLIB2;

    ENTER();

    __stdio_lock();

    if (__clib2->__num_iob > 0) {
        for (i = 0; i < __clib2->__num_iob; i++) {
            if (FLAG_IS_SET(__clib2->__iob[i]->iob_Flags, IOBF_IN_USE))
                fclose((FILE *) __clib2->__iob[i]);
        }

        __clib2->__num_iob = 0;
    }

    if (__clib2->__num_fd > 0) {
        for (i = 0; i < __clib2->__num_fd; i++) {
            if (FLAG_IS_SET(__clib2->__fd[i]->fd_Flags, FDF_IN_USE))
                close(i);
        }

        __clib2->__num_fd = 0;
    }

    __stdio_unlock();

    LEAVE();
}

STDIO_DESTRUCTOR(stdio_exit) {
    ENTER();

    __close_all_files();

    LEAVE();
}

STDIO_CONSTRUCTOR(stdio_init) {
    const int num_standard_files = (STDERR_FILENO - STDIN_FILENO + 1);
    BOOL success = FALSE;

    ENTER();

    if (__stdio_lock_init() < 0)
        goto out;

    if (__grow_iob_table(num_standard_files) < 0)
        goto out;

    if (__grow_fd_table(num_standard_files) < 0)
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
