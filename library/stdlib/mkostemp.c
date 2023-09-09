/*
 * $Id: stdlib_mkostemp.c,v 1.0 2023-07-13 12:04:26 clib2devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

int
mkostemp(char *template, int flags) {
    return mkostemps(template, 0, flags);
}
