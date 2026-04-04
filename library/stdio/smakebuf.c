/*
 * $Id: stdio_smakebuf.c,v 1.0 2025-01-01 00:00:00 clib4devs Exp $
 *
 * Lazy buffer allocation for FILE streams (newlib-inspired).
 * Called on first I/O operation rather than at fopen() time.
 */

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#ifndef _STDLIB_MEMORY_H
#include "stdlib_memory.h"
#endif /* _STDLIB_MEMORY_H */

/****************************************************************************/

/*
 * __swhatbuf — determine optimal buffer size.
 *
 * If the file descriptor backs a regular file, use stat.st_blksize
 * as a hint. Otherwise fall back to BUFSIZ.
 *
 * Returns the suggested buffer size and sets *couldbetty if the
 * fd is interactive (tty).
 */
static int
__swhatbuf(struct _clib4 *__clib4, struct iob *fp, int *couldbetty) {
    struct stat st;
    int fd;

    ENTER();

    *couldbetty = 0;
    fd = fp->iob_Descriptor;

    if (fd >= 0 && fd < __clib4->__num_fd && __clib4->__fd[fd] != NULL) {
        /* Check if interactive */
        if (FLAG_IS_SET(__clib4->__fd[fd]->fd_Flags, FDF_IS_INTERACTIVE)) {
            *couldbetty = 1;
        }

        /* Try to stat for block size.
         * AmigaOS returns st_blksize=512 (filesystem block size) which
         * is too small for efficient stdio — most reads/writes are multi-KB.
         * Enforce a minimum of 8192 to enable effective buffering. */
        if (fstat(fd, &st) == 0 && st.st_blksize > 0) {
            int blksize = (int) st.st_blksize;
            if (blksize < 8192)
                blksize = 8192;
            RETURN(blksize);
            return blksize;
        }
    }

    RETURN(BUFSIZ);
    return BUFSIZ;
}

/****************************************************************************/

/*
 * __smakebuf — allocate buffer for a FILE on first use.
 *
 * If the stream is unbuffered (_IONBF), set up the single-byte
 * fallback buffer (_nbuf). Otherwise allocate a buffer of the
 * appropriate size, aligned to cache line boundary.
 *
 * Sets IOBF_MALLOC_BUF flag if we malloc'd the buffer (so fclose
 * knows to free it).
 */
void
__smakebuf(struct _clib4 *__clib4, struct iob *fp) {
    int couldbetty;
    int bufsize;
    char *buf;
    int flags;

    ENTER();

    /* If buffer is already allocated, nothing to do */
    if (fp->iob_Buffer != NULL) {
        LEAVE();
        return;
    }

    flags = fp->iob_Flags & IOBF_BUFFER_MODE;

    if (flags == IOBF_BUFFER_MODE_NONE) {
        /* Unbuffered: use the single-byte internal buffer */
        fp->iob_Buffer = &fp->iob_SingleByte;
        fp->iob_BufferSize = 1;
        LEAVE();
        return;
    }

    /* Determine optimal buffer size */
    bufsize = __swhatbuf(__clib4, fp, &couldbetty);

    /* If interactive and not explicitly set to full buffering, use line buffering */
    if (couldbetty && flags == IOBF_BUFFER_MODE_FULL) {
        CLEAR_FLAG(fp->iob_Flags, IOBF_BUFFER_MODE);
        SET_FLAG(fp->iob_Flags, IOBF_BUFFER_MODE_LINE);
    }

    /* Allocate buffer, aligned to cache line boundary */
    if (__clib4->__cache_line_size > 1) {
        buf = (char *) malloc(bufsize + (__clib4->__cache_line_size - 1));
    } else {
        buf = (char *) malloc(bufsize);
    }

    if (buf == NULL) {
        /* Allocation failed — fall back to unbuffered with single byte */
        fp->iob_Buffer = &fp->iob_SingleByte;
        fp->iob_BufferSize = 1;
        CLEAR_FLAG(fp->iob_Flags, IOBF_BUFFER_MODE);
        SET_FLAG(fp->iob_Flags, IOBF_BUFFER_MODE_NONE);
        LEAVE();
        return;
    }

    fp->iob_CustomBuffer = buf;
    fp->iob_Buffer = (unsigned char *) buf;
    fp->iob_BufferSize = bufsize;
    fp->_blksize = bufsize;
    SET_FLAG(fp->iob_Flags, IOBF_MALLOC_BUF);

    LEAVE();
}
