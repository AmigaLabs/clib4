/*
 * $Id: stdio_filliobreadbuffer.c,v 1.12 2006-09-22 09:02:51 clib4devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

int
__fill_iob_read_buffer(struct _clib4 *__clib4, struct iob *file) {
    struct file_action_message fam;
    int num_bytes_read;
    int result = ERROR;

    ENTER();

    SHOWPOINTER(file);

    assert(FLAG_IS_SET(file->iob_Flags, IOBF_IN_USE));
    assert(file != NULL && (file->iob_BufferReadBytes == 0 || file->iob_BufferPosition == file->iob_BufferReadBytes) && file->iob_BufferWriteBytes == 0);
    assert(FLAG_IS_SET(file->iob_Flags, IOBF_READ));
    assert(file->iob_BufferSize > 0);

    /* Flush all line buffered streams before we proceed to fill this buffer. */
    if ((file->iob_Flags & IOBF_BUFFER_MODE) == IOBF_BUFFER_MODE_LINE) {
        if (__flush_all_files(__clib4, IOBF_BUFFER_MODE_LINE) < 0)
            goto out;
    }

    SHOWMSG("calling the hook");

    SHOWPOINTER(file->iob_Buffer);
    SHOWVALUE(file->iob_BufferSize);

    fam.fam_Action = file_action_read;
    fam.fam_Data = (char *) file->iob_Buffer;
    fam.fam_Size = file->iob_BufferSize;

    assert(file->iob_Action != NULL);

    num_bytes_read = (*file->iob_Action)(__clib4, file, &fam);
    if (num_bytes_read == EOF) {
        D(("got error %ld", fam.fam_Error));

        SET_FLAG(file->iob_Flags, IOBF_ERROR);

        __set_errno_r(__clib4, fam.fam_Error);

        goto out;
    }

    file->iob_BufferReadBytes = num_bytes_read;
    file->iob_BufferPosition = 0;

    SHOWVALUE(file->iob_BufferReadBytes);

    result = OK;

out:

    RETURN(result);
    return (result);
}
