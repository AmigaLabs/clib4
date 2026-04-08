/*
 * $Id: stdio_fgets.c,v 2.0 2025-01-01 00:00:00 clib4devs Exp $
 *
 * fgets() — newlib-inspired rewrite using __srefill() for buffer management.
 */

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

char *
fgets(char *buf, int n, FILE *stream) {
    struct iob *fp = (struct iob *) stream;
    struct _clib4 *__clib4 = __CLIB4;
    char *s;
    ssize_t r;
    unsigned char *p;

    ENTER();

    if (buf == NULL || stream == NULL) {
        __set_errno_r(__clib4, EFAULT);
        RETURN(NULL);
        return NULL;
    }

    if (n <= 0) {
        RETURN(NULL);
        return NULL;
    }

    __check_abort_f(__clib4);
    __flockfile_r(__clib4, stream);

    if (__fgetc_check(__clib4, stream) < 0) {
        buf = NULL;
        goto out;
    }

    /* Clear residual EOF/error flags so that ferror()/feof() after this
     * call reflect only errors from THIS read, not previous ones.
     * This matches the old fgets() behavior and prevents stale flags
     * from confusing callers like sqlite3's line reading loop. */
    CLEAR_FLAG(fp->iob_Flags, IOBF_EOF_REACHED);
    CLEAR_FLAG(fp->iob_Flags, IOBF_ERROR);

    s = buf;
    n--;    /* leave room for NUL terminator */

    while (n > 0) {
        /* Check if buffer has data. */
        r = READABLE_BYTES(fp);
        if (r <= 0) {
            /* Refill the buffer. */
            if (__srefill(__clib4, fp) != 0) {
                /* EOF or error. */
                if (s == buf)
                    buf = NULL;
                break;
            }
            r = READABLE_BYTES(fp);
        }

        /* Scan for newline in the buffered data. */
        if (n < r)
            r = n;
        p = memchr(READ_PTR(fp), '\n', (size_t)r);
        if (p != NULL) {
            /* Found newline — include it and stop. */
            r = (ssize_t)(p - READ_PTR(fp)) + 1;
            memcpy(s, READ_PTR(fp), (size_t)r);
            fp->iob_BufferPosition += r;
            s += r;
            break;
        }

        /* No newline found — copy everything and continue. */
        memcpy(s, READ_PTR(fp), (size_t)r);
        fp->iob_BufferPosition += r;
        s += r;
        n -= r;
    }

    *s = '\0';

out:
    __funlockfile_r(__clib4, stream);

    RETURN(buf);
    return buf;
}
