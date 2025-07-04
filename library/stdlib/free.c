/*
 * $Id: stdlib_free.c,v 1.15 2022-04-03 12:04:25 clib4devs Exp $
 */

/*#define DEBUG*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#include "mimalloc.h"

void free(void *ptr) { return mi_free(ptr); }
