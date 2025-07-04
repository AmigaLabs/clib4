/*
 * $Id: stdlib_memory.h,v 1.4 2006-01-08 12:04:26 clib4devs Exp $
*/

#ifndef _STDLIB_MEMORY_H
#define _STDLIB_MEMORY_H

#ifndef	EXEC_MEMORY_H
#include <exec/memory.h>
#endif /* EXEC_MEMORY_H */

#ifndef EXEC_NODES_H
#include <exec/nodes.h>
#endif /* EXEC_NODES_H */

/****************************************************************************/

#ifndef _MACROS_H
#include "macros.h"
#endif /* _MACROS_H */

#include <stddef.h>

#include "mimalloc.h"

/* We shuffle things around a bit for the debug code. This works by joining
   related code which shares the same name. The debug code symbols also have
   to be completely separate from the "regular" code. */

#define __static STATIC

// Magic number safe for all endianness
#define ALIGNED_BLOCK_MAGIC 0xABCD1234DCBA5678ULL

// Header structure with size optimized for 32/64-bit
typedef struct {
#if INTPTR_MAX == INT32_MAX
	uint32_t padding; // Maintain 8-byte alignment on 32-bit
#endif
	void *original_ptr;
	uint64_t magic;
} AlignedHeader;

extern void __memory_lock(struct _clib4 *__clib4);

extern void __memory_unlock(struct _clib4 *__clib4);

extern void __free_r(struct _clib4 *__clib4, void *ptr);

#endif /* _STDLIB_MEMORY_H */
