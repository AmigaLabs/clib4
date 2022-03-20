/*
 * $Id: stdio_fwrite.c,v 1.12 2010-10-20 13:12:58 clib2devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

size_t
fwrite(const void *ptr, size_t element_size, size_t count, FILE *stream) {
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

    if (FLAG_IS_CLEAR(file->iob_Flags, IOBF_WRITE)) {
        SHOWMSG("this stream is not write-enabled");

        SET_FLAG(file->iob_Flags, IOBF_ERROR);

        __set_errno(EBADF);

        goto out;
    }

    if (element_size > 0 && count > 0) {
        const unsigned char *data = (unsigned char *) ptr;
        unsigned char c;
        int buffer_mode;
        size_t total_bytes_written = 0;
        size_t total_size;

        total_size = element_size * count;

        if (__fputc_check((FILE *) file) < 0)
            goto out;

        buffer_mode = (file->iob_Flags & IOBF_BUFFER_MODE);
        if (buffer_mode == IOBF_BUFFER_MODE_NONE) {
            struct fd *fd = __fd[file->iob_Descriptor];

            __fd_lock(fd);

            if (FLAG_IS_SET(fd->fd_Flags, FDF_IS_INTERACTIVE))
                buffer_mode = IOBF_BUFFER_MODE_LINE;

            __fd_unlock(fd);
        }

        if (buffer_mode == IOBF_BUFFER_MODE_LINE) {
            while (total_size-- > 0) {
                c = (*data++);

                if (__putc_line_buffered(c, (FILE *) file) == EOF)
                    goto out;

                total_bytes_written++;
            }
        } else {
            while (total_size-- > 0) {
                c = (*data++);

                if (__putc_fully_buffered(c, (FILE *) file) == EOF)
                    goto out;

                total_bytes_written++;
            }
        }

        if ((file->iob_Flags & IOBF_BUFFER_MODE) == IOBF_BUFFER_MODE_NONE) {
            if (__iob_write_buffer_is_valid(file) && __flush_iob_write_buffer(file) < 0)
                goto out;
        }

        result = total_bytes_written / element_size;
    } else {
        /* Don't let this appear like an EOF or error. */
        clearerr((FILE *) file);
    }

out:

    funlockfile(stream);

    RETURN(result);
    return (result);
}
