/*
 * $Id: stdio_getline.c,v 1.0 2021-02-23 10:41:24 clib2devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

ssize_t
getline(char **lineptr, size_t *n, FILE *stream) {
    ENTER();

    ssize_t result = getdelim(lineptr, n, '\n', stream);

    LEAVE();
    return(result);
}
