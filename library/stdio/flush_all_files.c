/*
 * $Id: stdio_flush_all_files.c,v 1.3 2006-01-08 12:04:24 clib4devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

int
__flush_all_files(int buffer_mode) {
    int result;
    int i;
    struct _clib4 *__clib4 = __CLIB4;

    ENTER();
    SHOWVALUE(buffer_mode);

    __stdio_lock(__clib4);

    for (i = 0; i < __clib4->__num_iob; i++) {
        if (__clib4->__iob[i] != NULL &&
            FLAG_IS_SET(__clib4->__iob[i]->iob_Flags, IOBF_IN_USE) &&
            FLAG_IS_SET(__clib4->__iob[i]->iob_Flags, IOBF_WRITE) &&
            (buffer_mode < 0 || (__clib4->__iob[i]->iob_Flags & IOBF_BUFFER_MODE) == (ULONG) buffer_mode) &&
            __iob_write_buffer_is_valid(__clib4->__iob[i])) {
            if (__flush_iob_write_buffer(__clib4, __clib4->__iob[i]) < 0) {
                result = ERROR;
                goto out;
            }
        }
    }

    result = OK;

out:

    __stdio_unlock(__clib4);

    RETURN(result);
    return (result);
}
