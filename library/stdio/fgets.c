/*
 * $Id: stdio_fgets.c,v 1.7 2022-03-27 12:04:24 clib4devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

char *
fgets(char *s, int n, FILE *stream) {
    char *result = s;
    int c;
    struct _clib4 *__clib4 = __CLIB4;

    ENTER();
    SHOWPOINTER(s);
    SHOWVALUE(n);
    SHOWPOINTER(stream);

    if (s == NULL || stream == NULL) {
        SHOWMSG("invalid parameters");

        __set_errno(EFAULT);
        return NULL;
    }

    if (n <= 0) {
        SHOWMSG("no work to be done");
        return NULL;
    }

    flockfile(stream);

    /* Take care of the checks and data structure changes that
     * need to be handled only once for this stream.
     */
    if (__fgetc_check(stream, __clib4) < 0) {
        result = NULL;
        goto out;
    }

    /* So that we can tell error and 'end of file' conditions apart. */
    clearerr(stream);

    /* One off for the terminating '\0'. */
    n--;

    while (n-- > 0) {
        c = __getc(stream);
        if (c == EOF) {
            if (ferror(stream)) {
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

        (*s++) = c;

        if (c == '\n')
            break;
    }

    (*s) = '\0';
    SHOWSTRING(buf);

out:
    funlockfile(stream);

    RETURN(result);
    return (result);
}
