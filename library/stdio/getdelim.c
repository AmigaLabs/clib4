/*
 * $Id: stdio_getdelim.c,v 1.0 2021-02-23 10:42:24 clib2devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

ssize_t
getdelim(char **lineptr, size_t *n, int delimiter, FILE *stream) {
    char *ptr = *lineptr;
    size_t size = (ptr != NULL) ? *n : 0;
    size_t len = 0;

    ENTER();
    SHOWPOINTER(*lineptr);
    SHOWPOINTER(n);
    SHOWVALUE(delimiter);
    SHOWPOINTER(stream);

    for (;;) {
        if ((size - len) <= 2) {
            size = size ? (size * 2) : 256;
            ptr = realloc(*lineptr, size);
            if (ptr == NULL)
                return -1;
            *lineptr = ptr;
            *n = size;
        }

        int c = fgetc(stream);
        if (c == -1) {
            if (len == 0 || ferror(stream))
                return -1;
            break; /* EOF */
        }
        ptr[len++] = c;
        if (c == delimiter)
            break;
    }

    ptr[len] = '\0';

    RETURN(len);
    return len;
}
