/*
 * $Id: stdlib_free.c,v 1.15 2022-04-03 12:04:25 clib2devs Exp $
*/

/*#define DEBUG*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _STDLIB_MEMORY_H
#include "stdlib_memory.h"
#endif /* _STDLIB_MEMORY_H */

#include <malloc.h>

#undef free

void
free(void *ptr)
{
	__memory_lock();

    wof_free(__wof_allocator, ptr);

    __memory_unlock();
}
