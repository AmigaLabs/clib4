/*
 * $Id: stdio_fflush.c,v 1.11 2006-01-08 12:04:24 clib4devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

int
fflush(FILE *stream) {
    int result = EOF;
    struct _clib4 *__clib4 = __CLIB4;

    ENTER();

    SHOWPOINTER(stream);

    /* Flush a particular stream? */
    if (stream != NULL) {
        struct iob *file = (struct iob *) stream;

        assert(__is_valid_iob(__clib4, file));

        __flockfile_r(__clib4, stream);

        if (__iob_write_buffer_is_valid(file) && __flush_iob_write_buffer(__clib4, file) < 0) {
            __funlockfile_r(__clib4, stream);
            RETURN(result);
            return result;
        }

        __funlockfile_r(__clib4, stream);

    } else {
        int failed_iob = -1;
        int i;

        __stdio_lock(__clib4);

        /* Flush all streams which still have unwritten data in the buffer. */
        for (i = 0; i < __clib4->__num_iob; i++) {
            if (__clib4->__iob[i] != NULL &&
                FLAG_IS_SET(__clib4->__iob[i]->iob_Flags, IOBF_IN_USE) &&
                FLAG_IS_SET(__clib4->__iob[i]->iob_Flags, IOBF_WRITE) &&
                __iob_write_buffer_is_valid(__clib4->__iob[i])) {
                if (__flush_iob_write_buffer(__clib4, __clib4->__iob[i]) < 0) {
                    failed_iob = i;
                    break;
                }
            }
        }

        __stdio_unlock(__clib4);

        if (failed_iob >= 0)
            goto out;
    }

    result = OK;

out:

    RETURN(result);
    return (result);
}
