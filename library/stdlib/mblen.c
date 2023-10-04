/*
 * $Id: stdlib_mblen.c,v 1.3 2006-01-08 12:04:25 clib4devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

int
mblen(const char *s, size_t n) {
    ENTER();

    int result = mbtowc(0, s, n);

    RETURN(result);
    return result;
}
