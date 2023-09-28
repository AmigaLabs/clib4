/*
 * $Id: stdio_putchar.c,v 1.3 2006-01-08 12:04:25 clib4devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#undef putchar

int
putchar(int c) {
    int result;

    ENTER();

    result = fputc(c, stdout);

    LEAVE();
    return (result);
}
