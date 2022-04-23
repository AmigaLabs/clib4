/*
 * $Id: stdio_fclose.c,v 1.12 2006-01-08 12:04:24 clib2devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#ifndef _STDLIB_MEMORY_H
#include "stdlib_memory.h"
#endif /* _STDLIB_MEMORY_H */

/****************************************************************************/

int
fclose(FILE *stream) {
    struct iob *file = (struct iob *) stream;
    struct file_action_message fam;
    int result = OK;

    ENTER();

    SHOWPOINTER(stream);

    assert(stream != NULL);

    if (__check_abort_enabled)
        __check_abort();

    if (stream == NULL) {
        SHOWMSG("invalid stream parameter");

        result = EOF;

        __set_errno(EFAULT);
        goto out;
    }

    assert(__is_valid_iob(file));
    assert(FLAG_IS_SET(file->iob_Flags, IOBF_IN_USE));
    assert(file->iob_BufferSize > 0);

    if (FLAG_IS_CLEAR(file->iob_Flags, IOBF_IN_USE)) {
        SHOWMSG("this file is not even in use");

        result = EOF;

        __set_errno(EBADF);
        goto out;
    }

    /* Push back any buffered data to the stream. */
    if (__iob_write_buffer_is_valid(file) && __flush_iob_write_buffer(file) < 0)
        result = EOF;

    /* Make sure that the stream is closed. */
    SHOWMSG("calling the action function");

    fam.fam_Action = file_action_close;

    assert(file->iob_Action != NULL);

    if ((*file->iob_Action)(file, &fam) < 0 && result != EOF) {
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

        old_current_dir = CurrentDir(file->iob_TempFileLock);
        DeleteFile(file->iob_TempFileName);
        CurrentDir(old_current_dir);

        UnLock(file->iob_TempFileLock);

        free(file->iob_TempFileName);
    }

    /* Get rid of any custom file buffer allocated. */
    if (file->iob_CustomBuffer != NULL)
        free(file->iob_CustomBuffer);

    /* Free the lock semaphore now. */
    __delete_semaphore(file->iob_Lock);

    memset(file, 0, sizeof(*file));

out:
    RETURN(result);
    return (result);
}
