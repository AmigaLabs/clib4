/*
 * $Id: stdio_fopen64.c,v 1.0 2021-02-05 13:25:24 clib4devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

FILE *
fopen64(const char *filename, const char *mode) {
    ENTER();

    FILE *fp = fopen(filename, mode);
    if (fp != NULL) {
        fp->_flags |= __SL64;
    }

    LEAVE();

    return fp;
}
