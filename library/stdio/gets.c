/*
 * $Id: stdio_gets.c,v 1.7 2022-03-27 12:04:24 clib4devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

char *
gets(char *s) {
    FILE *stream = stdin;
    struct iob *file = (struct iob *) stream;
    char *result = s;
    int c;
    struct _clib4 *__clib4 = __CLIB4;

    ENTER();

    SHOWPOINTER(s);

    assert(s != NULL);

    flockfile(stream);

    if (s == NULL) {
        SHOWMSG("invalid parameters");

        __set_errno(EFAULT);

        result = NULL;
        goto out;
    }

    /* Take care of the checks and data structure changes that
     * need to be handled only once for this stream.
     */
    if (__fgetc_check(stream, __clib4) < 0) {
        result = NULL;
        goto out;
    }

    /* So that we can tell error and 'end of file' conditions apart. */
    clearerr(stream);

    while (TRUE) {
        c = __getc(stdin);
        if (c == EOF) {
            if (ferror(stdin)) {
                /* Just to be on the safe side. */
                (*s) = '\0';

                result = NULL;
                goto out;
            }

            /* Make sure that we return NULL if we really
               didn't read anything at all */
            if (s == result)
                result = NULL;

            break;
        }

        if (c == '\n')
            break;

        (*s++) = c;
    }

    (*s) = '\0';

    SHOWSTRING(result);

out:

    funlockfile(stream);

    RETURN(result);
    return (result);
}
