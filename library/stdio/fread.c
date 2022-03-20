/*
 * $Id: stdio_fread.c,v 1.7 2006-01-08 12:04:24 clib2devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

size_t
fread(void *ptr, size_t element_size, size_t count, FILE *stream) {
    struct iob *file = (struct iob *) stream;
    size_t result = 0;

    ENTER();

    SHOWPOINTER(ptr);
    SHOWVALUE(element_size);
    SHOWVALUE(count);
    SHOWPOINTER(stream);

    assert(ptr != NULL && stream != NULL);
    assert((int) element_size >= 0 && (int) count >= 0);

    if (__check_abort_enabled)
        __check_abort();

    flockfile(stream);

    if (ptr == NULL || stream == NULL) {
        SHOWMSG("invalid parameters");

        __set_errno(EFAULT);
        goto out;
    }

    assert(__is_valid_iob(file));
    assert(FLAG_IS_SET(file->iob_Flags, IOBF_IN_USE));
    assert(file->iob_BufferSize > 0);

    if (FLAG_IS_CLEAR(file->iob_Flags, IOBF_IN_USE)) {
        SHOWMSG("this file is not even in use");

        SET_FLAG(file->iob_Flags, IOBF_ERROR);

        __set_errno(EBADF);

        goto out;
    }

    if (FLAG_IS_CLEAR(file->iob_Flags, IOBF_READ)) {
        SHOWMSG("this file is not read-enabled");

        SET_FLAG(file->iob_Flags, IOBF_ERROR);

        __set_errno(EBADF);

        goto out;
    }

    if (element_size > 0 && count > 0) {
        size_t total_bytes_read = 0;
        size_t total_size;
        unsigned char *data = ptr;
        int c;

        if (__fgetc_check((FILE *) file) < 0)
            goto out;

        total_size = element_size * count;

        SHOWVALUE(total_size);

        while (total_size-- > 0) {
            c = __getc(file);
            if (c == EOF)
                break;

            (*data++) = c;

            total_bytes_read++;
        }

        SHOWVALUE(total_bytes_read);

        result = total_bytes_read / element_size;
    } else {
        SHOWVALUE(element_size);
        SHOWVALUE(count);

        SHOWMSG("either element size or count is zero");

        /* Don't let this appear like an EOF or error. */
        clearerr((FILE *) file);
    }

    D(("total number of elements read = %ld", result));

out:

    funlockfile(stream);

    RETURN(result);
    return (result);
}
