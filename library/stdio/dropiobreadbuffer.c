/*
 * $Id: stdio_dropiobreadbuffer.c,v 1.9 2006-01-08 12:04:24 clib4devs Exp $
*/

/* This would otherwise generate far too much (useless) debug output. */
#ifdef DEBUG
#undef DEBUG
#endif /* DEBUG */

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

int
__drop_iob_read_buffer(struct _clib4 *__clib4, struct iob *file) {
    int result = OK;

    ENTER();

    SHOWPOINTER(file);

    assert(file != NULL);

    assert(FLAG_IS_SET(file->iob_Flags, IOBF_IN_USE));
    assert(file->iob_BufferSize > 0);

    if (FLAG_IS_SET(file->iob_Flags, IOBF_IN_USE)) {
        CLEAR_FLAG(file->iob_Flags, IOBF_EOF_REACHED);

        if (__iob_read_buffer_is_valid(file)) {
            LONG num_unread_bytes = __iob_num_unread_bytes(file);

            D(("%ld bytes are to be dropped", num_unread_bytes));

            if (num_unread_bytes > 0) {
                struct file_action_message fam;
                int64_t position;

                SHOWMSG("calling the action function");

                fam.fam_Action = file_action_seek;
                fam.fam_Offset = -num_unread_bytes;
                fam.fam_Mode = SEEK_CUR;

                assert(file->iob_Action != NULL);

                position = (*file->iob_Action)(__clib4, file, &fam);
                if (fam.fam_Error != OK) {
                    SHOWMSG("that didn't work");

                    result = ERROR;

                    SET_FLAG(file->iob_Flags, IOBF_ERROR);

                    __set_errno(fam.fam_Error);

                    goto out;
                }

                /* If this is a valid file position, clear 'errno' so that
                   it cannot be mistaken for an error. */
                if (position < 0)
                    __set_errno(OK);
            }

            file->iob_BufferReadBytes = 0;
            file->iob_BufferPosition = 0;
        }
    }

out:

    RETURN(result);
    return (result);
}
