/*
 * $Id: stdio_freopen.c,v 1.5 2006-01-08 12:04:24 clib4devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

FILE *
freopen(const char *filename, const char *mode, FILE *stream) {
    struct iob *file = (struct iob *) stream;
    FILE *result = NULL;
    int slot_number;
    struct _clib4 *__clib4 = __CLIB4;

    ENTER();

    SHOWSTRING(filename);
    SHOWSTRING(mode);
    SHOWPOINTER(stream);

    assert(filename != NULL && mode != NULL && stream != NULL);

    __check_abort_f(__clib4);

    if (filename == NULL || mode == NULL || stream == NULL) {
        SHOWMSG("invalid parameters");

        __set_errno_r(__clib4, EFAULT);
        goto out;
    }

    assert(__is_valid_iob(__clib4, file));
    assert(FLAG_IS_SET(file->iob_Flags, IOBF_IN_USE));
    assert(file->iob_BufferSize > 0);

    /* We need to remember this; it'll go away when we close
       the file. */
    slot_number = file->iob_SlotNumber;

    fclose(stream);

    if (__open_iob(__clib4, filename, mode, -1, slot_number) < 0) {
        SHOWMSG("couldn't reopen the file");
        goto out;
    }

    result = (FILE *) file;
    /* Reset flags */
    result->_flags &= ~__SORD;
    result->_flags2 &= ~__SWID;
    memset(&result->_mbstate, 0, sizeof(_mbstate_t));

out:

    RETURN(result);
    return (result);
}
