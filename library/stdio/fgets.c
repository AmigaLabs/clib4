/*
 * $Id: stdio_fgets.c,v 1.7 2022-03-27 12:04:24 clib4devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

char *
fgets(char *buf, int n, FILE *stream) {
    struct iob *file = (struct iob *) stream;
    char *s = buf;
    int c;
    struct _clib4 *__clib4 = __CLIB4;

    ENTER();
    SHOWPOINTER(buf);
    SHOWVALUE(n);
    SHOWPOINTER(stream);

    if (buf == NULL || stream == NULL) {
        SHOWMSG("invalid parameters");

        __set_errno_r(__clib4, EFAULT);
        RETURN(NULL);
        return NULL;
    }

    if (n <= 0) {
        SHOWMSG("no work to be done");
        RETURN(NULL);
        return NULL;
    }

    __check_abort_f(__clib4);

    __flockfile_r(__clib4, stream);

    /* Take care of the checks and data structure changes that
     * need to be handled only once for this stream.
     */
    if (__fgetc_check(__clib4, stream) < 0) {
        buf = NULL;
        goto out;
    }

    /* So that we can tell error and 'end of file' conditions apart. */
    __clearerr_r(__clib4, stream);

    /* One off for the terminating '\0'. */
    n--;

    while (n > 0) {
        /* If there is data in the buffer, try to copy it directly
           into the string buffer. If there is a line feed in the
           buffer, too, try to conclude the read operation. */
        if (file->iob_BufferPosition < file->iob_BufferReadBytes) {
            const unsigned char *buffer = &file->iob_Buffer[file->iob_BufferPosition];
            size_t num_bytes_in_buffer;
            const unsigned char *lf;

            /* Copy only as much data as will fit into the string buffer. */
            num_bytes_in_buffer = (size_t) file->iob_BufferReadBytes - (size_t) file->iob_BufferPosition;
            if (num_bytes_in_buffer > (off_t) n)
                num_bytes_in_buffer = n;

            /* Try to find a line feed character which could conclude
               the read operation if the remaining buffer data, including
               the line feed character, fit into the string buffer. */
            lf = (unsigned char *) memchr(buffer, '\n', num_bytes_in_buffer);
            if (lf != NULL) {
                size_t num_characters_in_line = ++lf - buffer;

                /* Copy the remainder of the read buffer into the
                   string buffer, including the terminating line
                   feed character. */
                memmove(s, buffer, num_characters_in_line);
                s += num_characters_in_line;

                file->iob_BufferPosition += num_characters_in_line;

                /* And that concludes the line read operation. */
                (*s) = '\0';
                goto out;
            }

            memmove(s, buffer, num_bytes_in_buffer);
            s += num_bytes_in_buffer;

            file->iob_BufferPosition += num_bytes_in_buffer;

            /* Stop if the string buffer has been filled. */
            n -= num_bytes_in_buffer;
            if (n == 0)
                break;
        }

        /* Read the next buffered character; this will refill the read
           buffer, if necessary. */
        c = __getc(__clib4, stream);
        if (c == EOF) {
            if (__ferror_r(__clib4, stream, FALSE)) {
                /* Just to be on the safe side. */
                (*s) = '\0';

                buf = NULL;
                goto out;
            }

            /* Make sure that we return NULL if we really
               didn't read anything at all */
            if (buf == s)
                buf = NULL;

            break;
        }

        (*s++) = c;

        if (c == '\n')
            break;

        n--;
    }

    (*s) = '\0';
    SHOWSTRING(buf);

out:
    __funlockfile_r(__clib4, stream);

    RETURN(buf);
    return (buf);
}
