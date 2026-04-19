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
     * Walk the per-process glue list and close any dynamically allocated streams.
     * Skip the root node (which holds stdin/stdout/stderr) — those are handled below.
     */
    {
        struct _glue *root = (struct _glue *) __clib4->__sglue_root;
        if (root != NULL) {
            struct _glue *g;
            for (g = root->next; g != NULL; ) {
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
            root->next = NULL;

            /* Free the root glue node itself (per-process, allocated in file_init) */
            if (root->iobs != NULL)
                free(root->iobs);
            free(root);
            __clib4->__sglue_root = NULL;
        }
    }

    /* Close the per-process stdin/stdout/stderr streams (__iob[0..2]) */
    if (__clib4->__iob != NULL) {
        for (i = 0; i < 3 && i < __clib4->__num_iob; i++) {
            struct iob *fp = __clib4->__iob[i];
            if (fp != NULL && FLAG_IS_SET(fp->iob_Flags, IOBF_IN_USE)) {
                /* Flush the per-process iob */
                __sflush(__clib4, fp);
                CLEAR_FLAG(fp->iob_Flags, IOBF_IN_USE);
                if (fp->iob_CustomBuffer != NULL) {
                    if (fp->iob_isVBuffer)
                        FreeVec(fp->iob_CustomBuffer);
                    else
                        free(fp->iob_CustomBuffer);
                    fp->iob_CustomBuffer = NULL;
                }
                if (fp->iob_Lock != NULL) {
                    __delete_semaphore(fp->iob_Lock);
                    fp->iob_Lock = NULL;
                }
            }
        }
    }

    /* Free the __iob[] pointer table and its per-process iob structs.
     * Entries 0..2 are per-process (allocated by __grow_iob_table).
     * No entries beyond index 2 exist anymore since new streams are
     * allocated via __sfp() / glue list exclusively. */
    if (__clib4->__iob != NULL) {
        for (i = 0; i < __clib4->__num_iob; i++) {
            if (__clib4->__iob[i] != NULL) {
                __free_r(__clib4, __clib4->__iob[i]);
                __clib4->__iob[i] = NULL;
            }
        }
        __free_r(__clib4, __clib4->__iob);
        __clib4->__iob = NULL;
    }
    __clib4->__num_iob = 0;

    if (__clib4->__num_fd > 0) {
        for (i = 0; i < __clib4->__num_fd; i++) {
            /* If file is set as in use close it only if it isn't marked as FDF_NO_CLOSE */
            if ((i >= STDIN_FILENO || i < STDERR_FILENO) || (FLAG_IS_SET(__clib4->__fd[i]->fd_Flags, FDF_IN_USE) && FLAG_IS_CLEAR(__clib4->__fd[i]->fd_Flags, FDF_NO_CLOSE))) {
                D(("Close __fd %ld\n", i));
                close(i);
				SHOWMSG("Freeing Unlock memory");
                UnlockMem(__clib4->__fd[i], sizeof(*__clib4->__fd[i]));
				SHOWMSG("Freeing fd memory");
                __free_r(__clib4, __clib4->__fd[i]);
            }
            else {
                D(("Can't close __fd %ld FDF_STDIO=%ld FDF_IN_USE=%ld FDF_NO_CLOSE=%ld \n", i, FLAG_IS_SET(__clib4->__fd[i]->fd_Flags, FDF_STDIO), FLAG_IS_SET(__clib4->__fd[i]->fd_Flags, FDF_IN_USE), FLAG_IS_SET(__clib4->__fd[i]->fd_Flags, FDF_NO_CLOSE)));
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
