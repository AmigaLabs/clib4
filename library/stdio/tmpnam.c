/*
 * $Id: stdio_tmpnam.c,v 1.6 2006-01-08 12:04:25 clib4devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

char *
tmpnam(char *buf) {
    static char local_buffer[L_tmpnam];
    static unsigned long counter;

    APTR old_window_pointer;
    unsigned long c;
    char *result = NULL; /* ZZZ compiler claims that this assignment is unnecessary. */
    BPTR lock;
    int i;
    struct _clib4 *__clib4 = __CLIB4;

    ENTER();

    __check_abort_f(__clib4);

    /* If no user-supplied buffer is available, use the local one. */
    if (buf == NULL)
        buf = local_buffer;

    while (TRUE) {
        __check_abort_f(__clib4);

        c = counter;

        counter = (counter + 1) % TMP_MAX;

        /* Build another temporary file name, which begins with the
           letters 'tmp' followed by an octal number. */
        strcpy(buf, "tmp");

        /* There's room for L_tmpnam - 4 digits, which for
           L_tmpnam == 10 leaves room for 6 * 3 bits. */
        for (i = 3; i < L_tmpnam - 1; i++) {
            buf[i] = '0' + (c % 8);
            c = (c / 8);
        }

        buf[i] = '\0';

        D(("checking if '%s' exists", buf));

        /* Turn off DOS error requesters. */
        old_window_pointer = __set_process_window((APTR) - 1);

        /* Does this object exist already? */
        lock = Lock(buf, SHARED_LOCK);

        /* Restore DOS requesters. */
        __set_process_window(old_window_pointer);

        if (lock == BZERO) {
            /* If the object does not exist yet then we
               are finished. */
            if (IoErr() == ERROR_OBJECT_NOT_FOUND)
                result = buf;
            else
                __set_errno(__translate_io_error_to_errno(IoErr()));

            break;
        }

        /* OK, so it exists already. Start over... */
        UnLock(lock);
    }

    SHOWSTRING(result);

    RETURN(result);
    return (result);
}
