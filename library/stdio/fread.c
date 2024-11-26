/*
 * $Id: stdio_fread.c,v 1.8 2022-03-27 12:04:24 clib4devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

size_t
fread(void *ptr, size_t element_size, size_t count, FILE *stream) {
    struct iob *file = (struct iob *) stream;
    size_t result = 0;
    struct _clib4 *__clib4 = __CLIB4;

    ENTER();

    SHOWPOINTER(ptr);
    SHOWVALUE(element_size);
    SHOWVALUE(count);
    SHOWPOINTER(stream);

    assert(ptr != NULL && stream != NULL);
    assert((int) element_size >= 0 && (int) count >= 0);

    if (ptr == NULL || stream == NULL) {
        SHOWMSG("invalid parameters");

        __set_errno(EFAULT);
        RETURN(result);
        return (result);
    }

    __check_abort_f(__clib4);

    int locked = __ftrylockfile_r(__clib4, stream);

    assert(__is_valid_iob(__clib4, file));
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

    /* So that we can tell error and 'end of file' conditions apart. */
    __clearerr_r(__clib4, stream);

    if (element_size > 0 && count > 0) {
        size_t total_bytes_read = 0;
        size_t total_size;
        unsigned char *data = ptr;
        int c;

        if (__fgetc_check(__clib4, (FILE *) file) < 0)
            goto out;

        /* Check for overflow. */
        total_size = element_size * count;
        if (element_size != (total_size / count))
            goto out;

        SHOWVALUE(total_size);

        if ((file->iob_Flags & IOBF_BUFFER_MODE) == IOBF_BUFFER_MODE_NONE) {
            ssize_t num_bytes_read;

            /* We bypass the buffer entirely. */
            num_bytes_read = read(file->iob_Descriptor, data, total_size);
            if (num_bytes_read == -1) {
                SET_FLAG(file->iob_Flags, IOBF_ERROR);
                goto out;
            }

            if (num_bytes_read == 0)
                SET_FLAG(file->iob_Flags, IOBF_EOF_REACHED);

            total_bytes_read = num_bytes_read;
        } else {
            while (total_size > 0) {
                /* If there is more data to be read and the read buffer is empty
                   anyway, we'll bypass the buffer entirely. */
                if (file->iob_BufferReadBytes == 0 && total_size >= (size_t) file->iob_BufferSize) {
                    ssize_t num_bytes_read;

                    /* We bypass the buffer entirely. */
                    num_bytes_read = read(file->iob_Descriptor, data, total_size);
                    if (num_bytes_read == -1) {
                        SET_FLAG(file->iob_Flags, IOBF_ERROR);
                        goto out;
                    }

                    if (num_bytes_read == 0)
                        SET_FLAG(file->iob_Flags, IOBF_EOF_REACHED);

                    total_bytes_read += num_bytes_read;
                    break;
                }

                /* If there is data in the read buffer, try to copy it directly
                   into the output buffer. */
                if (file->iob_BufferPosition < file->iob_BufferReadBytes) {
                    const unsigned char *buffer = &file->iob_Buffer[file->iob_BufferPosition];
                    size_t num_bytes_in_buffer;

                    /* Copy as much data as will fit. */
                    num_bytes_in_buffer = file->iob_BufferReadBytes - file->iob_BufferPosition;
                    if (total_size < num_bytes_in_buffer)
                        num_bytes_in_buffer = total_size;

                    memmove(data, buffer, num_bytes_in_buffer);
                    data += num_bytes_in_buffer;

                    file->iob_BufferPosition += num_bytes_in_buffer;

                    total_bytes_read += num_bytes_in_buffer;

                    /* Stop if the string buffer has been filled. */
                    total_size -= num_bytes_in_buffer;
                    if (total_size == 0)
                        break;
                }

                c = __getc(__clib4, file);
                if (c == EOF)
                    break;

                (*data++) = c;

                total_size--;
                total_bytes_read++;
            }
        }
        SHOWVALUE(total_bytes_read);

        result = total_bytes_read / element_size;
    } else {
        SHOWVALUE(element_size);
        SHOWVALUE(count);

        SHOWMSG("either element size or count is zero");

    }

    D(("total number of elements read = %ld", result));

out:

    if (locked == OK)
        __funlockfile_r(__clib4, stream);

    RETURN(result);
    return (result);
}
