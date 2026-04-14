/*
 * $Id: stdio_fread.c,v 2.0 2025-01-01 00:00:00 clib4devs Exp $
 *
 * fread() — newlib-inspired rewrite using __srefill() for buffer management.
 */

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

size_t
__fread_internal(void *ptr, size_t element_size, size_t count, FILE *stream) {
    struct iob *fp = (struct iob *) stream;
    struct _clib4 *__clib4 = __CLIB4;
    size_t total_size;
    size_t total_bytes_read = 0;
    unsigned char *data = ptr;
    size_t r;           /* readable bytes in buffer */
    size_t result = 0;
    int locked;

    ENTER();

    SHOWPOINTER(ptr);
    SHOWVALUE(element_size);
    SHOWVALUE(count);
    SHOWPOINTER(stream);

    if (__builtin_expect(ptr == NULL || stream == NULL, 0)) {
        SHOWMSG("invalid parameters");
        __set_errno(EFAULT);
        RETURN(result);
        return result;
    }

    /* Compute total size; skip expensive division for common cases */
    if (__builtin_expect(element_size <= 1, 1)) {
        total_size = element_size * count;
    } else if (__builtin_expect(count <= 1, 1)) {
        total_size = element_size * count;
    } else {
        total_size = element_size * count;
        if ((total_size / element_size) != count) {
            RETURN(0);
            return 0;
        }
    }

    if (__builtin_expect(total_size == 0, 0)) {
        RETURN(0);
        return 0;
    }

    SHOWVALUE(total_size);

    /*
     * Inline lock — avoids __ftrylockfile_r function call overhead.
     * IOBF_LOCKED means the file was explicitly locked by flockfile();
     * in that case, skip the semaphore (caller holds it).
     */
    if (__builtin_expect(FLAG_IS_CLEAR(fp->iob_Flags, IOBF_LOCKED), 1)) {
        if (__builtin_expect(fp->iob_Lock != NULL, 1)) {
            ObtainSemaphore(fp->iob_Lock);
        }
        locked = OK;
    } else {
        locked = ERROR;  /* Already locked by flockfile() — don't unlock */
    }

    if (__builtin_expect(FLAG_IS_CLEAR(fp->iob_Flags, IOBF_IN_USE), 0)) {
        SHOWMSG("this file is not even in use");
        SET_FLAG(fp->iob_Flags, IOBF_ERROR);
        __set_errno(EBADF);
        goto out;
    }

    /*
     * Fast path 1: data already in buffer, no ungetc pending.
     * Covers the very common case of repeated small reads from
     * a pre-filled buffer. No signal check, no cantread, no smakebuf.
     */
    if (__builtin_expect(fp->iob_Buffer != NULL && !HASUB(fp), 1)) {
        r = fp->iob_BufferReadBytes - fp->iob_BufferPosition;
        if (__builtin_expect(r >= total_size, 1)) {
            memcpy(data, fp->iob_Buffer + fp->iob_BufferPosition, total_size);
            fp->iob_BufferPosition += total_size;
            result = count;
            goto out;
        }

        /*
         * Fast path 2: buffer is drained and request >= buffer size.
         * Read directly into the user buffer, bypassing the FILE buffer.
         * Skip cantread/smakebuf overhead — these only matter on first
         * use, and by now the buffer is already set up.
         */
        if (r == 0 && total_size >= (size_t)fp->iob_BufferSize
                    && fp->_read != NULL) {
            size_t remaining = total_size;
            while (remaining > 0) {
                ssize_t nr = fp->_read(fp->_cookie, (char *)data,
                                       (int)remaining);
                if (__builtin_expect(nr <= 0, 0)) {
                    if (nr == 0)
                        SET_FLAG(fp->iob_Flags, IOBF_EOF_REACHED);
                    else
                        SET_FLAG(fp->iob_Flags, IOBF_ERROR);
                    break;
                }
                data += nr;
                remaining -= nr;
            }
            result = (total_size - remaining) / element_size;
            goto out;
        }
    }

    /*
     * Slow path — only reached on first read (buffer not yet allocated),
     * ungetc pushback, or partial buffer drain.
     * Check CTRL-C here, not on every fast-path read.
     */
    __check_abort_f(__clib4);

    /* Slow path: need to check readability */
    if (__builtin_expect(cantread(__clib4, fp), 0)) {
        SHOWMSG("this file is not read-enabled");
        SET_FLAG(fp->iob_Flags, IOBF_ERROR);
        __set_errno(EBADF);
        goto out;
    }

    /* Ensure buffer is allocated (lazy init). */
    if (__builtin_expect(fp->iob_Buffer == NULL, 0))
        __smakebuf(__clib4, fp);

    /*
     * Main read loop — newlib-inspired:
     * 0. Drain ungetc pushback buffer first.
     * 1. Drain any buffered data.
     * 2. If remaining request is large (>= buffer size), bypass buffer
     *    and read directly into the user buffer via fp->_read().
     * 3. Otherwise, refill the buffer via __srefill() and copy.
     */

    /* Drain ungetc buffer first. */
    if (HASUB(fp) && fp->_ub._size > 0) {
        size_t ub_avail = fp->_ub._size;
        if (ub_avail > total_size)
            ub_avail = total_size;
        memcpy(data, fp->_ub._base, ub_avail);
        data += ub_avail;
        total_bytes_read += ub_avail;
        total_size -= ub_avail;
        fp->_ub._size -= ub_avail;
        if (fp->_ub._size > 0) {
            memmove(fp->_ub._base, fp->_ub._base + ub_avail, fp->_ub._size);
        } else {
            FREEUB(__clib4, fp);
            CLEAR_FLAG(fp->iob_Flags, IOBF_UNGETC);
        }
        if (total_size == 0)
            goto done;
    }

    while (total_size > 0) {
        /* Copy from buffer if data is available */
        r = READABLE_BYTES(fp);
        if (r > 0) {
            if (r > total_size)
                r = total_size;
            memcpy(data, READ_PTR(fp), r);
            fp->iob_BufferPosition += r;
            data += r;
            total_bytes_read += r;
            total_size -= r;
            if (total_size == 0)
                break;
        }

        /* Buffer is empty now. For large remaining reads, bypass the buffer. */
        if (total_size >= (size_t) fp->iob_BufferSize && fp->_read != NULL) {
            ssize_t nr;
            while (total_size > 0) {
                nr = fp->_read(fp->_cookie, (char *) data, (int) total_size);
                if (nr <= 0) {
                    if (nr == 0)
                        SET_FLAG(fp->iob_Flags, IOBF_EOF_REACHED);
                    else
                        SET_FLAG(fp->iob_Flags, IOBF_ERROR);
                    goto done;
                }
                data += nr;
                total_bytes_read += nr;
                total_size -= nr;
            }
            break;
        }

        /* Refill the buffer */
        if (__srefill(__clib4, fp) != 0)
            break;  /* EOF or error */
    }

done:
    SHOWVALUE(total_bytes_read);
    result = total_bytes_read / element_size;
    D(("total number of elements read = %ld", result));

out:
    /* Inline unlock — avoids __funlockfile_r function call overhead */
    if (locked == OK && __builtin_expect(fp->iob_Lock != NULL, 1)) {
        ReleaseSemaphore(fp->iob_Lock);
    }

    RETURN(result);
    return result;
}

static void byteswap16(void *ptr) {
    uint8_t *b = ptr;
    uint8_t t = b[0]; b[0] = b[1]; b[1] = t;
}

static void byteswap32(void *ptr) {
    uint8_t *b = ptr;
    uint8_t t;
    t = b[0]; b[0] = b[3]; b[3] = t;
    t = b[1]; b[1] = b[2]; b[2] = t;
}

static void byteswap64(void *ptr) {
    uint8_t *b = ptr;
    for (int i = 0; i < 4; ++i) {
        uint8_t t = b[i];
        b[i] = b[7 - i];
        b[7 - i] = t;
    }
}

size_t
fread(void *ptr, size_t element_size, size_t count, FILE *stream) {
    size_t total = element_size * count;
    size_t nread = __fread_internal(ptr, element_size, count, stream);
    struct iob *file = (struct iob *) stream;

    if (FLAG_IS_SET(file->iob_Flags, IOBF_LITTLE_ENDIAN) && (total == 2 || total == 4 || total == 8)) {
        DebugPrintF("[fread] Reading in Little endian mode\n");
        if (element_size == 2) {
            byteswap16(ptr);
        } else if (element_size == 4) {
            byteswap32(ptr);
        } else if (element_size == 8) {
            byteswap64(ptr);
        }
    }

    return nread;
}
