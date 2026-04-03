/*
 * $Id: stdio_fwrite.c,v 2.0 2025-01-01 00:00:00 clib4devs Exp $
 *
 * fwrite() — newlib-inspired rewrite using __swsetup()/__sflush() for buffer management.
 */

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

size_t
fwrite(const void *ptr, size_t element_size, size_t count, FILE *stream) {
    struct iob *fp = (struct iob *) stream;
    struct _clib4 *__clib4 = __CLIB4;
    size_t total_size;
    size_t total_bytes_written = 0;
    const unsigned char *s;
    size_t w;           /* writable space in buffer */
    int buffer_mode;
    size_t result = 0;

    ENTER();

    SHOWPOINTER(ptr);
    SHOWVALUE(element_size);
    SHOWVALUE(count);
    SHOWPOINTER(stream);

    assert(ptr != NULL && stream != NULL);

    if (ptr == NULL || stream == NULL) {
        SHOWMSG("invalid parameters");
        __set_errno_r(__clib4, EFAULT);
        RETURN(result);
        return result;
    }

    if (FLAG_IS_CLEAR(fp->iob_Flags, IOBF_IN_USE) || cantwrite(__clib4, fp)) {
        SET_FLAG(fp->iob_Flags, IOBF_ERROR);
        __set_errno_r(__clib4, EBADF);
        RETURN(result);
        return result;
    }

    /* Check for overflow. */
    total_size = element_size * count;
    if (total_size == 0) {
        RETURN(result);
        return result;
    }
    if (element_size != 0 && (total_size / element_size) != count) {
        RETURN(result);
        return result;
    }

    __flockfile_r(__clib4, stream);

    /* Prepare stream for writing (allocates buffer if needed). */
    if (__swsetup(__clib4, fp) != 0)
        goto out;

    s = (const unsigned char *) ptr;
    buffer_mode = (fp->iob_Flags & IOBF_BUFFER_MODE);

    /*
     * Unbuffered: write directly via fp->_write, one byte at a time
     * would be too slow, so we batch. For truly unbuffered, write
     * everything straight out.
     */
    if (buffer_mode == IOBF_BUFFER_MODE_NONE) {
        if (fp->_write != NULL) {
            while (total_size > 0) {
                ssize_t nw = fp->_write(fp->_cookie, (const char *) s, (int) total_size);
                if (nw <= 0) {
                    SET_FLAG(fp->iob_Flags, IOBF_ERROR);
                    goto done;
                }
                s += nw;
                total_bytes_written += nw;
                total_size -= nw;
            }
        }
        goto done;
    }

    /*
     * Buffered write loop (full or line buffered).
     */
    while (total_size > 0) {
        /* Large write with empty buffer: bypass buffer, write directly. */
        if (fp->iob_BufferWriteBytes == 0
            && total_size >= (size_t) fp->iob_BufferSize
            && buffer_mode != IOBF_BUFFER_MODE_LINE
            && fp->_write != NULL) {
            ssize_t nw = fp->_write(fp->_cookie, (const char *) s, (int) total_size);
            if (nw <= 0) {
                SET_FLAG(fp->iob_Flags, IOBF_ERROR);
                goto done;
            }
            s += nw;
            total_bytes_written += nw;
            total_size -= nw;
            continue;
        }

        /* Fill buffer. */
        w = WRITABLE_SPACE(fp);
        if (w > 0) {
            size_t n = (total_size < w) ? total_size : w;

            if (buffer_mode == IOBF_BUFFER_MODE_LINE) {
                /* For line buffered: look for newline to decide flush. */
                const unsigned char *lf = memchr(s, '\n', n);
                if (lf != NULL)
                    n = (size_t)(lf + 1 - s);
                memcpy(WRITE_PTR(fp), s, n);
                fp->iob_BufferWriteBytes += n;
                s += n;
                total_bytes_written += n;
                total_size -= n;

                if (lf != NULL || WRITABLE_SPACE(fp) == 0) {
                    if (__sflush(__clib4, fp) != 0)
                        goto done;
                }
                continue;
            }

            /* Full buffered: copy into buffer. */
            memcpy(WRITE_PTR(fp), s, n);
            fp->iob_BufferWriteBytes += n;
            s += n;
            total_bytes_written += n;
            total_size -= n;

            /* Flush if buffer is full. */
            if (WRITABLE_SPACE(fp) == 0) {
                if (__sflush(__clib4, fp) != 0)
                    goto done;
            }
            continue;
        }

        /* Buffer is full — flush and retry. */
        if (__sflush(__clib4, fp) != 0)
            goto done;
    }

done:
    SHOWVALUE(total_bytes_written);
    result = total_bytes_written / element_size;

out:
    __funlockfile_r(__clib4, stream);

    RETURN(result);
    return result;
}
