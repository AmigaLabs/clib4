/*
 * $Id: stdlib_valloc.c,v 1.0 2021-01-19 10:03:00 clib4devs Exp $
*/

/*#define DEBUG*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#include <malloc.h>

void *valloc(size_t size) {
    ENTER();
    SHOWVALUE(size);

    long pagesize = sysconf(_SC_PAGESIZE);

    if (pagesize < 0)
        pagesize = 4096;

    LEAVE();

    return (void *) memalign(pagesize, size);
}