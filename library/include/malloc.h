/*
 * $Id: malloc.h,v 1.0 2021-01-10 10:50:49 clib4devs Exp $
*/

#ifndef _MALLOC_H
#define _MALLOC_H

#include <features.h>

#include <stddef.h>
#include <stdio.h>

#define SIZE_ALIGN (4 * sizeof(size_t))
#define SIZE_MASK (-SIZE_ALIGN)

__BEGIN_DECLS

/* Allocate SIZE bytes allocated to ALIGNMENT bytes.  */
extern void *memalign(size_t alignment, size_t size);

/* Return the usable size of a block allocated by malloc(). */
extern size_t malloc_usable_size(void *ptr);

/* clib4 extension: dump the internal state of the per-process memory
 * allocator (every OS block, chunk and live allocation, plus summary
 * totals) on the serial port via DebugPrintF(). Intended for debugging
 * memory usage and leaks. */
extern void malloc_dump(void);

__END_DECLS

#endif //_MALLOC_H
