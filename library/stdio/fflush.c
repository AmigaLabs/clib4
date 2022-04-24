/*
 * $Id: stdio_fflush.c,v 1.11 2006-01-08 12:04:24 clib2devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

int
fflush(FILE *stream) {
    int result = EOF;

    ENTER();

    SHOWPOINTER(stream);

    /* Flush a particular stream? */
    if (stream != NULL) {
        struct iob *file = (struct iob *) stream;

        assert(__is_valid_iob(file));

        flockfile(stream);

        if (__iob_write_buffer_is_valid(file) && __flush_iob_write_buffer(file) < 0)
            goto out;
    } else {
        int failed_iob = -1;
        int i;

        __stdio_lock();

        /* Flush all streams which still have unwritten data in the buffer. */
        for (i = 0; i < __num_iob; i++) {
            if (__iob[i] != NULL &&
                FLAG_IS_SET(__iob[i]->iob_Flags, IOBF_IN_USE) &&
                FLAG_IS_SET(__iob[i]->iob_Flags, IOBF_WRITE) &&
                __iob_write_buffer_is_valid(__iob[i])) {
                if (__flush_iob_write_buffer(__iob[i]) < 0) {
                    failed_iob = i;
                    break;
                }
            }
        }

        __stdio_unlock();

        if (failed_iob >= 0)
            goto out;
    }

    result = OK;

out:

    if (stream != NULL)
        funlockfile(stream);

    RETURN(result);
    return (result);
}
