/*
 * $Id: stdio_filliobreadbuffer.c,v 1.13 2025-01-01 09:02:51 clib4devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

int
__fill_iob_read_buffer(struct _clib4 *__clib4, struct iob *file) {
    int result;

    ENTER();

    SHOWPOINTER(file);

    assert(FLAG_IS_SET(file->iob_Flags, IOBF_IN_USE));
    assert(file != NULL);
    assert(FLAG_IS_SET(file->iob_Flags, IOBF_READ));

    /* Flush all line buffered streams before we proceed to fill this buffer. */
    if ((file->iob_Flags & IOBF_BUFFER_MODE) == IOBF_BUFFER_MODE_LINE) {
        if (__flush_all_files(__clib4, IOBF_BUFFER_MODE_LINE) < 0) {
            RETURN(ERROR);
            return ERROR;
        }
    }

    /* Use the new __srefill() path */
    result = __srefill(__clib4, file);

    SHOWVALUE(file->iob_BufferReadBytes);

    RETURN(result == 0 ? OK : ERROR);
    return (result == 0 ? OK : ERROR);
}
