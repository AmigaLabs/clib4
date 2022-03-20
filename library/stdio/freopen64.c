/*
 * $Id: stdio_freopen64.c,v 1.0 2021-02-06 17:13:24 clib2devs Exp $
*/

#define _GNU_SOURCE

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

FILE *
freopen64(const char *filename, const char *mode, FILE *stream) {
    FILE *fp = freopen(filename, mode, stream);
    
    if (fp != NULL) {
        fp->_flags |= __SL64;
    }
    
    return fp;
}