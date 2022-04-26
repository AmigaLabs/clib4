/*
 * $Id: stdio_ungetc.c,v 1.7 2006-01-08 12:04:25 clib2devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

int
ungetc(int c, FILE *stream) {
    struct iob *file = (struct iob *) stream;
    int result = EOF;

    ENTER();
    SHOWVALUE(c);
    SHOWPOINTER(stream);

    assert(stream != NULL);

    flockfile(stream);

    if (stream == NULL) {
        SHOWMSG("null file pointer!");

        __set_errno(EFAULT);
        goto out;
    }

    assert(__is_valid_iob(file));
    assert(FLAG_IS_SET(file->iob_Flags, IOBF_IN_USE));
    assert(file->iob_BufferSize > 0);

    if (FLAG_IS_CLEAR(file->iob_Flags, IOBF_IN_USE)) {
        SHOWMSG("this file is not even in use");

        SET_FLAG(file->iob_Flags, IOBF_ERROR);

        __set_errno(EBADF);

        goto out;
    }

    /* Pushing back characters only works for files that can be read from. */
    if (FLAG_IS_CLEAR(file->iob_Flags, IOBF_READ)) {
        SHOWMSG("can't even read from this file");

        SET_FLAG(file->iob_Flags, IOBF_ERROR);

        __set_errno(EACCES);

        goto out;
    }

    /* Pushing back an EOF is forbidden. */
    if (c == EOF) {
        SHOWMSG("cannot push back an EOF");

        SET_FLAG(file->iob_Flags, IOBF_ERROR);

        __set_errno(EINVAL);

        goto out;
    }

    /* Get rid of the write buffer, if it's still around. */
    if (__iob_write_buffer_is_valid(file) > 0 && __flush_iob_write_buffer(file) < 0) {
        SHOWMSG("could not flush write buffer");
        goto out;
    }

    /* We need to replace one of the characters in the buffer, which must
     * have been read before. The ISO standard requires that it must
     * be possible to push back at least one single character.
     */
    if (file->iob_BufferPosition == 0) {
        SHOWMSG("no room to push back");

        SET_FLAG(file->iob_Flags, IOBF_ERROR);

        __set_errno(ENOBUFS);

        goto out;
    }

    CLEAR_FLAG(file->iob_Flags, IOBF_EOF_REACHED);

    /* Replace the character just read. */
    file->iob_Buffer[--file->iob_BufferPosition] = c;

    /* Clamp the result to an unsigned 8 bit value. */
    result = (c & 255);

out:

    funlockfile(stream);

    RETURN(result);
    return (result);
}
