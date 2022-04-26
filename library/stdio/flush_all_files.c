/*
 * $Id: stdio_flush_all_files.c,v 1.3 2006-01-08 12:04:24 clib2devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

int
__flush_all_files(int buffer_mode) {
    int result;
    int i;

    ENTER();
    SHOWVALUE(buffer_mode);

    __stdio_lock();

    for (i = 0; i < __num_iob; i++) {
        if (__iob[i] != NULL &&
            FLAG_IS_SET(__iob[i]->iob_Flags, IOBF_IN_USE) &&
            FLAG_IS_SET(__iob[i]->iob_Flags, IOBF_WRITE) &&
            (buffer_mode < 0 || (__iob[i]->iob_Flags & IOBF_BUFFER_MODE) == (ULONG) buffer_mode) &&
            __iob_write_buffer_is_valid(__iob[i])) {
            if (__flush_iob_write_buffer(__iob[i]) < 0) {
                result = ERROR;
                goto out;
            }
        }
    }

    result = OK;

out:

    __stdio_unlock();

    RETURN(result);
    return (result);
}
