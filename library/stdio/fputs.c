/*
 * $Id: stdio_fputs.c,v 2.0 2025-01-01 00:00:00 clib4devs Exp $
 *
 * fputs() — newlib-inspired rewrite using __swsetup()/__sflush().
 */

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

int
fputs(const char *s, FILE *stream) {
    struct _clib4 *__clib4 = __CLIB4;
    return __fputs_r(__clib4, s, stream);
}

int
__fputs_r(struct _clib4 *__clib4, const char *s, FILE *stream) {
    struct iob *fp = (struct iob *) stream;
    size_t total_size;
    int buffer_mode;
    size_t w;
    int result = EOF;

    ENTER();

    SHOWSTRING(s);
    SHOWPOINTER(stream);

    if (s == NULL || stream == NULL) {
        __set_errno(__clib4 ? EFAULT : EFAULT);
        RETURN(result);
        return result;
    }

    __flockfile_r(__clib4, stream);

    if (__fputc_check(__clib4, stream) < 0)
        goto out;

    total_size = strlen(s);
    if (total_size == 0) {
        result = OK;
        goto out;
    }

    buffer_mode = (fp->iob_Flags & IOBF_BUFFER_MODE);

    /*
     * Unbuffered: write directly via fp->_write.
     */
    if (buffer_mode == IOBF_BUFFER_MODE_NONE) {
        if (fp->_write != NULL) {
            const unsigned char *p = (const unsigned char *) s;
            size_t remaining = total_size;
            while (remaining > 0) {
                ssize_t nw = fp->_write(fp->_cookie, (const char *) p, (int) remaining);
                if (nw <= 0) {
                    SET_FLAG(fp->iob_Flags, IOBF_ERROR);
                    goto out;
                }
                p += nw;
                remaining -= nw;
            }
        }
        result = OK;
        goto out;
    }

    /*
     * Buffered write loop.
     */
    while (total_size > 0) {
        w = WRITABLE_SPACE(fp);
        if (w > 0) {
            size_t n = (total_size < w) ? total_size : w;

            if (buffer_mode == IOBF_BUFFER_MODE_LINE) {
                const unsigned char *lf = memchr(s, '\n', n);
                if (lf != NULL)
                    n = (size_t)(lf + 1 - (const unsigned char *)s);
                memcpy(WRITE_PTR(fp), s, n);
                fp->iob_BufferWriteBytes += n;
                s += n;
                total_size -= n;

                if (lf != NULL || WRITABLE_SPACE(fp) == 0) {
                    if (__sflush(__clib4, fp) != 0)
                        goto out;
                }
                continue;
            }

            /* Full buffered. */
            memcpy(WRITE_PTR(fp), s, n);
            fp->iob_BufferWriteBytes += n;
            s += n;
            total_size -= n;

            if (WRITABLE_SPACE(fp) == 0) {
                if (__sflush(__clib4, fp) != 0)
                    goto out;
            }
            continue;
        }

        /* Buffer full — flush and retry. */
        if (__sflush(__clib4, fp) != 0)
            goto out;
    }

    result = OK;

out:
    __funlockfile_r(__clib4, stream);

    RETURN(result);
    return result;
}
