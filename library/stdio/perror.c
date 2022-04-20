/*
 * $Id: stdio_perror.c,v 1.4 2006-01-08 12:04:24 clib2devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

void
perror(const char *s) {
    char *error_message;
    int error = __get_errno();

    if (s != NULL)
        fprintf(stderr, "%s: ", s);

    error_message = strerror(error);

    if (error_message != NULL)
        fprintf(stderr, "%s", error_message);
    else
        fprintf(stderr, "error %d", error);

    fprintf(stderr, "\n");
}
