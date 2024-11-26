/*
 * $Id: stdio_fclose.c,v 1.13 2024-07-20 12:04:24 clib4devs Exp $
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
    struct file_action_message fam;
    int result = OK;
    struct _clib4 *__clib4 = __CLIB4;

    ENTER();

    SHOWPOINTER(stream);

    assert(stream != NULL);

    DECLARE_UTILITYBASE();

    __check_abort_f(__clib4);

    if (stream == NULL) {
        SHOWMSG("invalid stream parameter");

        result = EOF;

        __set_errno(EFAULT);
        goto out;
    }

    assert(__is_valid_iob(__clib4, file));
    assert(FLAG_IS_SET(file->iob_Flags, IOBF_IN_USE));
    assert(file->iob_BufferSize > 0);

    if (FLAG_IS_CLEAR(file->iob_Flags, IOBF_IN_USE)) {
        SHOWMSG("this file is not even in use");

        result = EOF;

        __set_errno(EBADF);
        goto out;
    }

    /* Push back any buffered data to the stream. */
    if (__iob_write_buffer_is_valid(file) && __flush_iob_write_buffer(__clib4, file) < 0)
        result = EOF;

    /* Make sure that the stream is closed. */
    SHOWMSG("calling the action function");

    fam.fam_Action = file_action_close;

    assert(file->iob_Action != NULL);

    if ((*file->iob_Action)(__clib4, file, &fam) < 0 && result != EOF) {
        result = EOF;

        __set_errno(fam.fam_Error);
    }

    /* Now that the file is closed and we are in fact
     * dealing with a temporary file, delete that file.
     */
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

    /* Get rid of any custom file buffer allocated. */
    if (file->iob_CustomBuffer != NULL) {
        SHOWMSG("Delete allocated buffer");
        FreeVec(file->iob_CustomBuffer);
        file->iob_CustomBuffer = NULL;
    }

    /* Free the lock semaphore now. */
    SHOWMSG("Delete iob_Lock");
    __delete_semaphore(file->iob_Lock);

    SHOWMSG("Clear file structure");
    memset(file, 0, sizeof(*file));

out:
    RETURN(result);
    return (result);
}
