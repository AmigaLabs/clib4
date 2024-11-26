/*
 * $Id: stdio_setbuf.c,v 1.5 2006-01-08 12:04:25 clib4devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

void
setbuf(FILE *stream, char *buf) {
    ENTER();

    SHOWPOINTER(stream);
    SHOWPOINTER(buf);

    assert(stream != NULL);

    __check_abort();

    if (stream == NULL) {
        SHOWMSG("invalid stream parameter");

        __set_errno(EFAULT);
        goto out;
    }

    if (buf == NULL)
        setvbuf(stream, NULL, IOBF_BUFFER_MODE_NONE, 0);
    else
        setvbuf(stream, buf, IOBF_BUFFER_MODE_FULL, BUFSIZ);

out:

    LEAVE();
}
