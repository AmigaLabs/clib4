/*
 * $Id: stdio_perror.c,v 1.4 2006-01-08 12:04:24 clib4devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

void
perror(const char *s) {
    char *error_message;
    struct _clib4 *__clib4 = __CLIB4;
    int error = __get_errno();

    ENTER();
    SHOWSTRING(s);

    if (s != NULL)
        fprintf(__stderr_r(__clib4), "%s: ", s);

    error_message = strerror(error);

    if (error_message != NULL)
        fprintf(__stderr_r(__clib4), "%s", error_message);
    else
        fprintf(__stderr_r(__clib4), "error %d", error);

    fprintf(__stderr_r(__clib4), "\n");

    LEAVE();
}
