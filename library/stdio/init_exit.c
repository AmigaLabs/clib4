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
     * First, walk the glue list and close any open streams.
     * Skip the static __sf[0..2] (stdin/stdout/stderr) as they are
     * handled by the old __iob[] path below for backward compat.
     * Only close dynamically allocated streams from __sfp().
     */
    {
        struct _glue *g;
        for (g = __sglue.next; g != NULL; ) {
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
        __sglue.next = NULL;
    }

    /* Close the static stdin/stdout/stderr streams via fclose */
    for (i = 0; i < 3; i++) {
        if (FLAG_IS_SET(__sf[i].iob_Flags, IOBF_IN_USE)) {
            /* Flush but don't free the static structs */
            __sflush(__clib4, &__sf[i]);
            CLEAR_FLAG(__sf[i].iob_Flags, IOBF_IN_USE);
            if (__sf[i].iob_CustomBuffer != NULL) {
                if (__sf[i].iob_isVBuffer)
                    FreeVec(__sf[i].iob_CustomBuffer);
                else
                    free(__sf[i].iob_CustomBuffer);
                __sf[i].iob_CustomBuffer = NULL;
            }
            if (__sf[i].iob_Lock != NULL) {
                __delete_semaphore(__sf[i].iob_Lock);
                __sf[i].iob_Lock = NULL;
            }
        }
    }

    /* Free the __iob[] pointer table (entries 0..2 point to static __sf[],
     * which were already cleaned up above — do NOT free those structs).
     * No entries beyond index 2 exist anymore since new streams are
     * allocated via __sfp() / glue list exclusively. */
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
