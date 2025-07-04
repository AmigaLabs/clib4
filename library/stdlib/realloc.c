/*
 * $Id: stdlib_realloc.c,v 1.11 2022-04-03 10:55:03 clib4devs Exp $
*/

/*#define DEBUG*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#include "mimalloc.h"

void *
realloc(void *ptr, size_t size) {
    return mi_realloc(ptr, size);
}
