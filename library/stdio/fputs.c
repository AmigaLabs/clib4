/*
 * $Id: stdio_fputs.c,v 1.8 2022-03-27 12:04:24 clib4devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#ifndef _FCNTL_HEADERS_H
#include "fcntl_headers.h"
#endif /* _FCNTL_HEADERS_H */

int
fputs(const char *s, FILE *stream) {
    struct iob *file = (struct iob *) stream;
    size_t total_size;
    int result = EOF;
    int c;
    struct _clib4 *__clib4 = __CLIB4;

    ENTER();

    SHOWSTRING(s);
    SHOWPOINTER(stream);

    assert(s != NULL && stream != NULL);

    if (s == NULL || stream == NULL) {
        SHOWMSG("invalid parameters");
        __set_errno(EFAULT);

        RETURN(result);
        return result;
    }

    __check_abort_f(__clib4);

    __flockfile_r(__clib4, stream);

    assert(__is_valid_iob(__clib4, file));
    assert(FLAG_IS_SET(file->iob_Flags, IOBF_IN_USE));
    assert(file->iob_BufferSize > 0);

    if (__fputc_check(__clib4, stream) < 0)
        goto out;

    total_size = strlen(s);
    if (total_size > 0) {
        int buffer_mode;

        /* We perform line buffering to improve readability of the
           buffered text if buffering was disabled and the output
           goes to an interactive stream. */
        buffer_mode = (file->iob_Flags & IOBF_BUFFER_MODE);
        if (buffer_mode == IOBF_BUFFER_MODE_NONE) {
            struct fd *fd = __clib4->__fd[file->iob_Descriptor];

            __fd_lock(fd);

            if (FLAG_IS_SET(fd->fd_Flags, FDF_IS_INTERACTIVE))
                buffer_mode = IOBF_BUFFER_MODE_LINE;

            __fd_unlock(fd);
        }

        if (buffer_mode == IOBF_BUFFER_MODE_LINE) {
            while (total_size > 0) {
                /* Is there still room in the write buffer to store
                   more of the string? */
                if (file->iob_BufferWriteBytes < file->iob_BufferSize) {
                    unsigned char *buffer = &file->iob_Buffer[file->iob_BufferWriteBytes];
                    size_t num_buffer_bytes;
                    const char *lf;

                    /* Store only as many characters as will fit into the write buffer. */
                    num_buffer_bytes = file->iob_BufferSize - file->iob_BufferWriteBytes;
                    if (total_size < num_buffer_bytes)
                        num_buffer_bytes = total_size;

                    /* Try to find a line feed in the string. If there is one,
                       reduce the number of characters to write to the sequence
                       which ends with the line feed character. */
                    lf = memchr(s, '\n', num_buffer_bytes);
                    if (lf != NULL)
                        num_buffer_bytes = lf + 1 - s;

                    memmove(buffer, s, num_buffer_bytes);
                    s += num_buffer_bytes;

                    file->iob_BufferWriteBytes += num_buffer_bytes;

                    /* Write the buffer to disk if it's full or contains a line feed. */
                    if ((lf != NULL || __iob_write_buffer_is_full(file)) && __flush_iob_write_buffer(__clib4, file) < 0) {
                        /* Abort with error. */
                        goto out;
                    }

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
                    goto out;

                total_size--;
            }
        } else if (buffer_mode == IOBF_BUFFER_MODE_NONE) {
            ssize_t num_bytes_written;

            /* We bypass the buffer entirely. */
            num_bytes_written = write(file->iob_Descriptor, s, total_size);
            if (num_bytes_written == -1) {
                SET_FLAG(file->iob_Flags, IOBF_ERROR);
                goto out;
            }
        } else {
            while (total_size > 0) {
                /* If there is more data to be written than the write buffer will hold
                   and the write buffer is empty anyway, then we'll bypass the write
                   buffer entirely. */
                if (file->iob_BufferWriteBytes == 0 && total_size >= (size_t) file->iob_BufferSize) {
                    ssize_t num_bytes_written;

                    /* We bypass the buffer entirely. */
                    num_bytes_written = __write_r(__clib4, file->iob_Descriptor, s, total_size);
                    if (num_bytes_written == -1) {
                        SET_FLAG(file->iob_Flags, IOBF_ERROR);
                        goto out;
                    }

                    break;
                }

                /* Is there still room in the write buffer to store
                   more of the string? */
                if (file->iob_BufferWriteBytes < file->iob_BufferSize) {
                    unsigned char *buffer = &file->iob_Buffer[file->iob_BufferWriteBytes];
                    size_t num_buffer_bytes;

                    /* Store only as many characters as will fit into the write buffer. */
                    num_buffer_bytes = file->iob_BufferSize - file->iob_BufferWriteBytes;
                    if (total_size < num_buffer_bytes)
                        num_buffer_bytes = total_size;

                    memmove(buffer, s, num_buffer_bytes);
                    s += num_buffer_bytes;

                    file->iob_BufferWriteBytes += num_buffer_bytes;

                    /* Write a full buffer to disk. */
                    if (__iob_write_buffer_is_full(file) && __flush_iob_write_buffer(__clib4, file) < 0) {
                        /* Abort with error. */
                        goto out;
                    }

                    /* Stop as soon as no further data needs to be written. */
                    total_size -= num_buffer_bytes;
                    if (total_size == 0)
                        break;

                    /* If there is again room in the output buffer,
                       try this optimization again. */
                    if (file->iob_BufferWriteBytes < file->iob_BufferSize)
                        continue;
                }

                c = (*s++);

                if (__putc_fully_buffered(__clib4, c, (FILE *) file) == EOF)
                    goto out;

                total_size--;
            }
        }
    }

    result = OK;

out:

    /* Note: if buffering is disabled for this stream, then we still
       may have buffered data around, queued to be printed right now.
       This is intended to improve performance as it takes more effort
       to write a single character to a file than to write a bunch. */
    if (result == 0 && (file->iob_Flags & IOBF_BUFFER_MODE) == IOBF_BUFFER_MODE_NONE) {
        if (__iob_write_buffer_is_valid(file) && __flush_iob_write_buffer(__clib4, file) < 0) {
            SHOWMSG("couldn't flush the write buffer");
            result = EOF;
        }
    }

    __funlockfile_r(__clib4, stream);

    RETURN(result);
    return (result);
}
