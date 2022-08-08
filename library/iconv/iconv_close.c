/*
 * $Id: iconv_close.c,v 1.0 2021-03-09 12:04:25 clib2devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#include <iconv.h>

int
iconv_close(iconv_t cd) {
    if (!((size_t) cd & 1))
        free((void *) cd);
    return 0;
}