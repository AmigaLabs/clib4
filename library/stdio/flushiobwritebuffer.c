/*
 * $Id: stdio_flushiobwritebuffer.c,v 1.9 2006-09-22 09:02:51 clib4devs Exp $
*/

/* This would otherwise generate far too much (useless) debug output. */
#ifdef DEBUG
#undef DEBUG
#endif /* DEBUG */

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

int
__flush_iob_write_buffer(struct _clib4 *__clib4, struct iob *file) {
    int result = OK;

    ENTER();

    SHOWPOINTER(file);

    assert(file != NULL);
    assert(FLAG_IS_SET(file->iob_Flags, IOBF_IN_USE));
    assert(file->iob_BufferSize > 0);

    if (FLAG_IS_SET(file->iob_Flags, IOBF_IN_USE) && file->iob_BufferWriteBytes > 0) {
        struct file_action_message fam;

        assert(FLAG_IS_SET(file->iob_Flags, IOBF_WRITE));
        assert(file->iob_BufferSize > 0);

        D(("%ld bytes are to be written", file->iob_BufferWriteBytes));

        SHOWMSG("calling the hook");

        fam.fam_Action = file_action_write;
        fam.fam_Data = (char *) file->iob_Buffer;
        fam.fam_Size = file->iob_BufferWriteBytes;

        assert(file->iob_Action != NULL);

        if ((*file->iob_Action)(__clib4, file, &fam) == EOF) {
            SHOWMSG("that didn't work");
            result = ERROR;
            SET_FLAG(file->iob_Flags, IOBF_ERROR);
            __set_errno_r(__clib4, fam.fam_Error);
            goto out;
        }

        file->iob_BufferWriteBytes = 0;
    }

out:

    RETURN(result);
    return (result);
}
