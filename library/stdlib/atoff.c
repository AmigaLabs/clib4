/*
 * $Id: stdlib_atoff.c,v 1.0 2021-03-29 16:34:25 clib2devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

float
atoff(const char *nptr) {
    return strtof(nptr, NULL);
}
