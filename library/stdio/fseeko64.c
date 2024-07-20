/*
 * $Id: stdio_fseeko64.c,v 1.0 2021-02-05 13:57:14 clib4devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

_off64_t
fseeko64(FILE *stream, _off64_t offset, int wherefrom) {
    struct iob *file = (struct iob *) stream;
    _off64_t result = CHANGE_FILE_ERROR;
    struct _clib4 *__clib4 = __CLIB4;

    ENTER();

    SHOWPOINTER(stream);
    SHOWVALUE(offset);
    SHOWVALUE(wherefrom);

    assert(stream != NULL);

    if (stream == NULL) {
        SHOWMSG("invalid stream parameter");

        __set_errno_r(__clib4, EFAULT);
        RETURN(result);
        return (result);
    }

    __check_abort_f(__clib4);

    __flockfile_r(__clib4, stream);

    if (wherefrom < SEEK_SET || wherefrom > SEEK_END) {
        SHOWMSG("invalid wherefrom parameter");
        SET_FLAG(file->iob_Flags, IOBF_ERROR);

        __set_errno_r(__clib4, EBADF);
        goto out;
    }

    assert(__is_valid_iob(__clib4, file));
    assert(FLAG_IS_SET(file->iob_Flags, IOBF_IN_USE));
    assert(file->iob_BufferSize > 0);

    if (FLAG_IS_CLEAR(file->iob_Flags, IOBF_IN_USE)) {
        SHOWMSG("this file is not even in use");
        SET_FLAG(file->iob_Flags, IOBF_ERROR);

        __set_errno_r(__clib4, EBADF);
        goto out;
    }

    /* Try to turn the absolute position into a relative seek
	 * within the buffer, if possible.
	 */
    if (wherefrom == SEEK_SET && __iob_read_buffer_is_valid(file)) {
        _off64_t current_position;

        current_position = ftello64(stream);
        if (current_position != CHANGE_FILE_ERROR || __get_errno() == OK) {
            offset -= current_position;

            wherefrom = SEEK_CUR;
        }
    }

    /* We have to clear the EOF condition */
    CLEAR_FLAG(file->iob_Flags, IOBF_EOF_REACHED);

    if (wherefrom != SEEK_CUR || offset != 0) {
        BOOL buffer_position_adjusted = FALSE;

        /* Try to adjust the buffer position rather than adjusting
		 * the file position itself, which is very costly.
		 */
        if (wherefrom == SEEK_CUR && __iob_read_buffer_is_valid(file)) {
            /* Try to adjust the buffer read position. This also
			 * affects the number of bytes that can still be read.
			 */
            if ((offset < 0 && (-offset) <= file->iob_BufferPosition) ||
                (offset > 0 && offset <= __iob_num_unread_bytes(file))) {
                file->iob_BufferPosition += offset;

                buffer_position_adjusted = TRUE;
            }
        }

        if (NOT buffer_position_adjusted) {
            struct file_action_message fam;
            _off64_t position;

            /* Oh dear, no luck. So we have to get rid of the
			 * current buffer contents and start with a clean
			 * slate.
			 */
            if (__iob_write_buffer_is_valid(file) && __flush_iob_write_buffer(__clib4, file) < 0) {
                SHOWMSG("couldn't flush write buffer");

                goto out;
            }

            if (__iob_read_buffer_is_valid(file) && __drop_iob_read_buffer(__clib4, file) < 0) {
                SHOWMSG("couldn't drop read buffer");

                goto out;
            }

            SHOWMSG("calling the hook");

            SHOWPOINTER(&fam);

            fam.fam_Action = file_action_seek;
            fam.fam_Offset = offset;
            fam.fam_Mode = wherefrom;

            SHOWVALUE(fam.fam_Offset);
            SHOWVALUE(fam.fam_Mode);

            assert(file->iob_Action != NULL);

            position = (*file->iob_Action)(__clib4, file, &fam);
            if (fam.fam_Error != OK) {
                SET_FLAG(file->iob_Flags, IOBF_ERROR);

                __set_errno_r(__clib4, fam.fam_Error);

                goto out;
            }

            /* If this is a valid file position, clear 'errno' so that
			   it cannot be mistaken for an error. */
            if (position < 0)
                __set_errno_r(__clib4, OK);
        }
    }

    result = OK;

out:

    __funlockfile_r(__clib4, stream);

    RETURN(result);
    return (result);
}
