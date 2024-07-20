/*
 * $Id: stdio_setvbuf.c,v 1.12 2022-03-27 12:07:58 clib4devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#ifndef _STDLIB_MEMORY_H
#include "stdlib_memory.h"
#endif /* _STDLIB_MEMORY_H */

int
setvbuf(FILE *stream, char *buf, int bufmode, size_t size) {
    struct iob *file = (struct iob *) stream;
    char *new_buffer = NULL;
    int result = EOF;
    struct _clib4 *__clib4 = __CLIB4;

    ENTER();

    SHOWPOINTER(stream);
    SHOWPOINTER(buf);
    SHOWVALUE(bufmode);
    SHOWVALUE(size);

    assert(stream != NULL);

    if (stream == NULL) {
        SHOWMSG("invalid stream parameter");
        __set_errno_r(__clib4, EFAULT);

        RETURN(result);
        return (result);
    }

    if (bufmode < IOBF_BUFFER_MODE_FULL ||
        bufmode > IOBF_BUFFER_MODE_NONE) {
        SHOWMSG("invalid buffer mode");

        RETURN(result);
        return (result);
    }

    if ((int) size < 0) {
        SHOWMSG("invalid buffer size");

        __set_errno_r(__clib4, EINVAL);
        RETURN(result);
        return (result);
    }

    __flockfile_r(__clib4, stream);

    assert(__is_valid_iob(__clib4, file));
    assert(FLAG_IS_SET(file->iob_Flags, IOBF_IN_USE));
    assert(file->iob_BufferSize > 0);

    if (FLAG_IS_CLEAR(file->iob_Flags, IOBF_IN_USE)) {
        SHOWMSG("this file is not even in use");
        SET_FLAG(file->iob_Flags, IOBF_ERROR);
        __set_errno_r(__clib4, EBADF);
        goto out;
    }

    /* A buffer size of 0 bytes will cause the default buffer size to be used. */
    if (size == 0) {
        size = BUFSIZ;
        buf = NULL;
    }

    /* If a certain buffer size is requested but no buffer was provided,
       allocate some memory for it. */
    if (bufmode != IOBF_BUFFER_MODE_NONE) {
        /* If a certain buffer size is requested but no buffer was provided,
		   allocate some memory for it. */
        if (size > 0 && buf == NULL) {
            /* Allocate a little more memory than necessary. */
            new_buffer = AllocVecTags(size + (__clib4->__cache_line_size - 1), AVT_Type, MEMF_SHARED, AVT_Alignment, __clib4->__cache_line_size, TAG_DONE);
            if (new_buffer == NULL) {
                __set_errno_r(__clib4, ENOBUFS);
                goto out;
            }
        }
    }

    /* Get rid of any buffered data. We're going to replace the buffer. */
    if (__iob_write_buffer_is_valid(file) && __flush_iob_write_buffer(__clib4, file) < 0) {
        SHOWMSG("could not flush write buffer");
        goto out;
    }

    if (__iob_read_buffer_is_valid(file) && __drop_iob_read_buffer(__clib4, file) < 0) {
        SHOWMSG("could not drop read buffer");
        goto out;
    }

    /* Get rid of any buffer specially allocated for this stream. */
    if (file->iob_CustomBuffer != NULL) {
        SHOWMSG("Delete allocated buffer");
        FreeVec(file->iob_CustomBuffer);
        file->iob_CustomBuffer = NULL;
    }

    if (bufmode == IOBF_BUFFER_MODE_NONE) {
        /* Use the default buffer, which is one byte in size. */
        new_buffer = (char *) &file->iob_SingleByte;
        size = 1;
    } else {
        /* Replace the old buffer. */
        if (buf != NULL) {
            new_buffer = (char *) buf;
        } else {
            /* Remember this, so we can release it later. */
            file->iob_CustomBuffer = new_buffer;

            /* Align the buffer start address to a cache line boundary. */
            new_buffer = (char *) ((ULONG)(new_buffer + (__clib4->__cache_line_size - 1)) & ~(__clib4->__cache_line_size - 1));
        }
    }

    assert(file->iob_BufferPosition == 0);
    assert(file->iob_BufferReadBytes == 0);
    assert(file->iob_BufferWriteBytes == 0);

    file->iob_Buffer = (UBYTE *) new_buffer;
    file->iob_BufferSize = size;

    file->iob_Flags &= ~IOBF_BUFFER_MODE;
    file->iob_Flags |= bufmode;

    new_buffer = NULL;

    result = OK;

out:

    __funlockfile_r(__clib4, stream);

    if (new_buffer != NULL)
        __free_r(__clib4, new_buffer);

    RETURN(result);
    return (result);
}
