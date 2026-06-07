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

    /*
     * First, walk the dynamic part of the glue list (beyond stdin/stdout/stderr)
     * and close any open streams allocated via __sfp().
     */
    if (__clib4->__sglue != NULL) {
        struct _glue *g;
        for (g = __clib4->__sglue->next; g != NULL; ) {
            struct _glue *next_g = g->next;
            for (i = 0; i < g->niobs; i++) {
                if (g->iobs[i] != NULL && FLAG_IS_SET(g->iobs[i]->iob_Flags, IOBF_IN_USE)) {
                    D(("Close glue iob %ld\n", i));
                    fclose((FILE *) g->iobs[i]);
                }
            }
            /* Free the iob array, pointer array, and glue block */
            if (g->iobs != NULL) {
                if (g->niobs > 0 && g->iobs[0] != NULL)
                    free(g->iobs[0]);  /* Free the contiguous iob array */
                free(g->iobs);         /* Free the pointer array */
            }
            free(g);
            g = next_g;
        }
        __clib4->__sglue->next = NULL;
    }

    /* Close and free the per-process stdin/stdout/stderr iob structs. */
    for (i = 0; i < 3; i++) {
        struct iob *iob = __clib4->__sf[i];
        if (iob != NULL) {
            if (FLAG_IS_SET(iob->iob_Flags, IOBF_IN_USE)) {
                __sflush(__clib4, iob);
                CLEAR_FLAG(iob->iob_Flags, IOBF_IN_USE);
                if (iob->iob_CustomBuffer != NULL) {
                    if (iob->iob_isVBuffer)
                        FreeVec(iob->iob_CustomBuffer);
                    else
                        free(iob->iob_CustomBuffer);
                    iob->iob_CustomBuffer = NULL;
                }
                if (iob->iob_Lock != NULL) {
                    __delete_semaphore(iob->iob_Lock);
                    iob->iob_Lock = NULL;
                }
            }
            free(iob);
            __clib4->__sf[i] = NULL;
        }
    }

    /* Free the root glue node (its iobs array is __clib4->__sf, not separately allocated). */
    if (__clib4->__sglue != NULL) {
        free(__clib4->__sglue);
        __clib4->__sglue = NULL;
    }
    if (__clib4->__iob != NULL) {
        __free_r(__clib4, __clib4->__iob);
        __clib4->__iob = NULL;
    }
    __clib4->__num_iob = 0;

    if (__clib4->__num_fd > 0) {
        for (i = 0; i < __clib4->__num_fd; i++) {
            struct fd *fd = __clib4->__fd[i];
            BOOL is_stdio_fd = (i >= STDIN_FILENO && i <= STDERR_FILENO);

            if (fd == NULL)
                continue;

            /* Close stdio fds 0..2 unconditionally, and any live non-NO_CLOSE descriptor. */
            if (is_stdio_fd ||
                (FLAG_IS_SET(fd->fd_Flags, FDF_IN_USE) &&
                 FLAG_IS_CLEAR(fd->fd_Flags, FDF_NO_CLOSE))) {
                D(("Close __fd %ld\n", i));
                close(i);
                SHOWMSG("Freeing Unlock memory");
                UnlockMem(fd, sizeof(*fd));
                SHOWMSG("Freeing fd memory");
                __free_r(__clib4, fd);
                __clib4->__fd[i] = NULL;
            }
            else {
                D(("Can't close __fd %ld FDF_STDIO=%ld FDF_IN_USE=%ld FDF_NO_CLOSE=%ld \n", i, FLAG_IS_SET(fd->fd_Flags, FDF_STDIO), FLAG_IS_SET(fd->fd_Flags, FDF_IN_USE), FLAG_IS_SET(fd->fd_Flags, FDF_NO_CLOSE)));
            }
        }
        __clib4->__num_fd = 0;

        /* Free the fd table itself */
        if (__clib4->__fd != NULL) {
            __free_r(__clib4, __clib4->__fd);
            __clib4->__fd = NULL;
        }
    }

    __clib4->__stdio_initialized = 0;

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
