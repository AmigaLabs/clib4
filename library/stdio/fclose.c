/*
 * $Id: stdio_fclose.c,v 1.14 2025-01-01 12:04:24 clib4devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#ifndef _STDLIB_MEMORY_H
#include "stdlib_memory.h"
#endif /* _STDLIB_MEMORY_H */

int
fclose(FILE *stream) {
    struct iob *file = (struct iob *) stream;
    int result = OK;
    struct _clib4 *__clib4 = __CLIB4;

    ENTER();

    SHOWPOINTER(stream);

    assert(stream != NULL);

    if (stream == NULL) {
        SHOWMSG("invalid stream parameter");
        result = EOF;
        __set_errno(EFAULT);
        goto out;
    }

    if (FLAG_IS_CLEAR(file->iob_Flags, IOBF_IN_USE)) {
        SHOWMSG("this file is not even in use");
        result = EOF;
        __set_errno(EBADF);
        goto out;
    }

    /* Flush any pending write data via the new unified flush */
    if (__sflush(__clib4, file) != 0)
        result = EOF;

    /* Close the underlying fd.
     * Prefer the new function pointer path; fall back to legacy iob_Action. */
    if (file->_close != NULL) {
        SHOWMSG("closing via _close function pointer");
        if (file->_close(file->_cookie) < 0 && result != EOF) {
            result = EOF;
        }
    } else if (file->iob_Action != NULL) {
        /* Legacy path (transition) */
        struct file_action_message fam;
        SHOWMSG("closing via legacy iob_Action");
        fam.fam_Action = file_action_close;
        if ((*file->iob_Action)(__clib4, file, &fam) < 0 && result != EOF) {
            result = EOF;
            __set_errno(fam.fam_Error);
        }
    }

    /* Handle temporary files */
    if (FLAG_IS_SET(file->iob_Flags, IOBF_TEMP)) {
        BPTR old_current_dir;

        SHOWMSG("this is a temporary file; deleting it");

        assert(file->iob_TempFileName != NULL);

        old_current_dir = SetCurrentDir(file->iob_TempFileLock);
        Delete(file->iob_TempFileName);
        SetCurrentDir(old_current_dir);

        UnLock(file->iob_TempFileLock);

        __free_r(__clib4, file->iob_TempFileName);
    }

    /* Free any ungetc pushback buffer */
    if (HASUB(file)) {
        FREEUB(__clib4, file);
    }

    /* Free any line buffer scratch */
    if (HASLB(file)) {
        FREELB(__clib4, file);
    }

    /* Get rid of any custom/malloc'd file buffer */
    if (file->iob_CustomBuffer != NULL) {
        SHOWMSG("Delete allocated buffer");
        if (file->iob_isVBuffer)
            FreeVec(file->iob_CustomBuffer);
        else
            free(file->iob_CustomBuffer);
        file->iob_CustomBuffer = NULL;
    }

    /* Free the lock semaphore now. */
    SHOWMSG("Delete iob_Lock");
    __delete_semaphore(file->iob_Lock);

    /* Clear the structure, marking it as free for reuse by __sfp() */
    SHOWMSG("Clear file structure");
    memset(file, 0, sizeof(*file));

out:
    RETURN(result);
    return (result);
}
