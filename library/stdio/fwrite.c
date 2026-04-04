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
    unsigned long flags;

    ENTER();

    SHOWPOINTER(ptr);
    SHOWVALUE(element_size);
    SHOWVALUE(count);
    SHOWPOINTER(stream);

    if (__builtin_expect(ptr == NULL || stream == NULL, 0)) {
        SHOWMSG("invalid parameters");
        __set_errno_r(__clib4, EFAULT);
        RETURN(result);
        return result;
    }

    /* Compute total size. Avoid expensive PPC integer division for the
     * overwhelmingly common case of element_size==1 or count==1. */
    if (__builtin_expect(element_size <= 1, 1)) {
        total_size = element_size * count; /* 0 or count — can't overflow */
    } else if (__builtin_expect(count <= 1, 1)) {
        total_size = element_size * count; /* 0 or element_size — can't overflow */
    } else {
        total_size = element_size * count;
        if ((total_size / element_size) != count) {
            RETURN(result);
            return result;
        }
    }

    if (__builtin_expect(total_size == 0, 0)) {
        RETURN(result);
        return result;
    }

    __flockfile_r(__clib4, stream);

    flags = fp->iob_Flags;

    if (__builtin_expect(FLAG_IS_CLEAR(flags, IOBF_IN_USE), 0)) {
        SET_FLAG(fp->iob_Flags, IOBF_ERROR);
        __set_errno_r(__clib4, EBADF);
        goto out;
    }

    /*
     * Ultra-fast path: fully-buffered stream with buffer ready + data fits.
     * Combines the __swsetup skip, buffer_mode check, and write into a
     * single branch sequence — the common case for repeated small writes.
     */
    buffer_mode = (flags & IOBF_BUFFER_MODE);

    if (__builtin_expect(
            buffer_mode == IOBF_BUFFER_MODE_FULL &&
            fp->iob_Buffer != NULL &&
            fp->iob_BufferReadBytes == 0, 1)) {
        w = fp->iob_BufferSize - fp->iob_BufferWriteBytes;
        if (__builtin_expect(total_size <= w, 1)) {
            if (total_size == 1) {
                fp->iob_Buffer[fp->iob_BufferWriteBytes++] = *(const unsigned char *)ptr;
            } else {
                memcpy(fp->iob_Buffer + fp->iob_BufferWriteBytes, ptr, total_size);
                fp->iob_BufferWriteBytes += total_size;
            }
            result = count;
            goto out;
        }
    }

    /*
     * Medium path: buffer is ready but needs __swsetup-style checks
     * (e.g. line-buffered, or buffer full).
     */
    if (fp->iob_Buffer != NULL && fp->iob_BufferReadBytes == 0 && !HASUB(fp)) {
        /* Already set up for writing — skip __swsetup. */
    } else {
        /* Slow path: first write, or mode switch needed. */
        if (__swsetup(__clib4, fp) != 0)
            goto out;
        buffer_mode = (fp->iob_Flags & IOBF_BUFFER_MODE);
    }

    s = (const unsigned char *) ptr;

    /*
     * Unbuffered: write directly via fp->_write.
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
