/*
 * $Id: stdio_fwrite.c,v 1.14 2024-07-20 13:12:58 clib4devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#ifndef _FCNTL_HEADERS_H
#include "fcntl_headers.h"
#endif /* _FCNTL_HEADERS_H */

size_t
fwrite(const void *ptr, size_t element_size, size_t count, FILE *stream) {
    struct iob *file = (struct iob *) stream;
    size_t result = 0; //EOF??;
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

        __set_errno_r(__clib4, EFAULT);
        RETURN(result);
        return (result);
    }

    if (FLAG_IS_CLEAR(file->iob_Flags, IOBF_IN_USE) || FLAG_IS_CLEAR(file->iob_Flags, IOBF_WRITE)) {
        SHOWMSG("this file is not even in use or write enabled");

        SET_FLAG(file->iob_Flags, IOBF_ERROR);
        SET_FLAG(file->iob_Flags2, __SERR);

        __set_errno_r(__clib4, EBADF);
        RETURN(result);
        return (result);
    }

    __check_abort_f(__clib4);

    __flockfile_r(__clib4, stream);

    if (element_size > 0 && count > 0) {
        const unsigned char *s = (unsigned char *) ptr;
        unsigned char c;
        int buffer_mode;
        size_t total_bytes_written = 0;
        size_t total_size;

        /* Check for overflow. */
        total_size = element_size * count;
        if (element_size != (total_size / count))
            goto out;

        /* If this is an unbuffered interactive stream, we will switch
           to line buffered mode in order to improve readability of
           the output. */
        buffer_mode = (file->iob_Flags & IOBF_BUFFER_MODE);
        if (buffer_mode == IOBF_BUFFER_MODE_NONE) {
            struct fd *fd = __clib4->__fd[file->iob_Descriptor];

            __fd_lock(fd);

            if (FLAG_IS_SET(fd->fd_Flags, FDF_IS_INTERACTIVE))
                buffer_mode = IOBF_BUFFER_MODE_LINE;

            __fd_unlock(fd);
        }
        switch (buffer_mode) {
            case IOBF_BUFFER_MODE_LINE:
                SHOWMSG("IOBF_BUFFER_MODE_LINE");
                while (total_size > 0) {
                    /* Is there still room in the write buffer to store more of the data? */
                    if (file->iob_BufferWriteBytes < file->iob_BufferSize) {
                        unsigned char *buffer = &file->iob_Buffer[file->iob_BufferWriteBytes];
                        size_t num_buffer_bytes;
                        const unsigned char *lf;

                        /* Store only as many characters as will fit into the write buffer. */
                        num_buffer_bytes = file->iob_BufferSize - file->iob_BufferWriteBytes;
                        if (total_size < num_buffer_bytes)
                            num_buffer_bytes = total_size;

                        /* Try to find a line feed in the string. If there is one,
                           reduce the number of characters to write to the sequence
                           which ends with the line feed character. */
                        lf = (unsigned char *) memchr(s, '\n', num_buffer_bytes);
                        if (lf != NULL)
                            num_buffer_bytes = lf + 1 - s;

                        memmove(buffer, s, num_buffer_bytes);
                        s += num_buffer_bytes;

                        file->iob_BufferWriteBytes += num_buffer_bytes;

                        /* Write the buffer to disk if it's full or contains a line feed. */
                        if ((lf != NULL || __iob_write_buffer_is_full(file)) && __flush_iob_write_buffer(__clib4, file) < 0) {
                            /* Abort with error. */
                            break;
                        }

                        total_bytes_written += num_buffer_bytes;

                        /* Stop as soon as no further data needs to be written. */
                        total_size -= num_buffer_bytes;
                        if (total_size == 0)
                            break;

                        /* If there is again room in the output buffer,
                           repeat this optimization. */
                        if (file->iob_BufferWriteBytes < file->iob_BufferSize)
                            continue;
                    }

                    c = (*s++);

                    if (__putc_line_buffered(__clib4, c, (FILE *) file) == EOF)
                        break;

                    total_size--;
                    total_bytes_written++;
                }
            break;
            case IOBF_BUFFER_MODE_NONE: {
                SHOWMSG("IOBF_BUFFER_MODE_NONE");
                ssize_t num_bytes_written;

                /* We bypass the buffer entirely. */
                num_bytes_written = __write_r(__clib4, file->iob_Descriptor, s, total_size);
                if (num_bytes_written == -1) {
                    SET_FLAG(file->iob_Flags, IOBF_ERROR);
                    goto out;
                }

                total_bytes_written = (size_t) num_bytes_written;

                if (__iob_write_buffer_is_valid(file)) {
                    __flush_iob_write_buffer(__clib4, file);
                }
            }
            break;
            default: {
                SHOWMSG("IOBF_BUFFER_MODE_FULL");
                while (total_size > 0) {
                    SHOWVALUE(file->iob_BufferWriteBytes);
                    SHOWVALUE(file->iob_BufferSize - file->iob_BufferWriteBytes);
                    SHOWVALUE(total_size);
                    /* If there is more data to be written than the write buffer will hold
                       and the write buffer is empty anyway, then we'll bypass the write
                       buffer entirely. */
                    if (file->iob_BufferWriteBytes == 0 && total_size >= (size_t) file->iob_BufferSize) {
                        SHOWMSG("bypass the buffer entirely");
                        /* We bypass the buffer entirely. */
                        ssize_t num_bytes_written = __write_r(__clib4, file->iob_Descriptor, s, total_size);
                        if (num_bytes_written == -1) {
                            SET_FLAG(file->iob_Flags, IOBF_ERROR);
                            goto out;
                        }

                        total_bytes_written += num_bytes_written;
                        break;
                    }
                    SHOWVALUE(file->iob_BufferWriteBytes);
                    SHOWVALUE(file->iob_BufferSize - file->iob_BufferWriteBytes);
                    /* Is there still room in the write buffer to store more of the data? */
                    if (file->iob_BufferWriteBytes < file->iob_BufferSize) {
                        unsigned char *buffer = &file->iob_Buffer[file->iob_BufferWriteBytes];
                        /* Store only as many bytes as will fit into the write buffer. */
                        size_t num_buffer_bytes = file->iob_BufferSize - file->iob_BufferWriteBytes;
                        if (total_size < num_buffer_bytes)
                            num_buffer_bytes = total_size;

                        memcpy(buffer, s, num_buffer_bytes);
                        s += num_buffer_bytes;

                        file->iob_BufferWriteBytes += num_buffer_bytes;

                        /* Write a full buffer to disk. */
                        if (__iob_write_buffer_is_full(file) && __flush_iob_write_buffer(__clib4, file) < 0) {
                            /* Abort with error. */
                            break;
                        }

                        total_bytes_written += num_buffer_bytes;
                        SHOWVALUE(total_bytes_written);

                        /* Stop as soon as no further data needs to be written. */
                        total_size -= num_buffer_bytes;
                        if (total_size == 0)
                            break;

                        /* If there is again room in the output buffer, try this optimization again. */
                        if (file->iob_BufferWriteBytes < file->iob_BufferSize)
                            continue;
                    }

                    c = (*s++);
                    SHOWMSG("__putc_fully_buffered");
                    if (__putc_fully_buffered(__clib4, c, (FILE *) file) == EOF)
                        goto out;

                    total_bytes_written++;
                }
            }
        }

        result = total_bytes_written / element_size;
    } else {
        SHOWMSG("either element size or count is zero");
    }

out:
    __funlockfile_r(__clib4, stream);

    RETURN(result);
    return (result);
}
