/*
 * $Id: stdlib_memory.h,v 1.4 2006-01-08 12:04:26 clib2devs Exp $
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

/* We shuffle things around a bit for the debug code. This works by joining
   related code which shares the same name. The debug code symbols also have
   to be completely separate from the "regular" code. */

#define __static STATIC

/* If this flag is set in mn_Size, then this memory allocation
 * cannot be released with free() or used with realloc(). This
 * flag is set by alloca().
 */
#define MN_SIZE_NEVERFREE (0x80000000UL)

/* This obtains the allocation size from a memory node, ignoring
 * the "never free" flag altogether.
 */
#define GET_MN_SIZE(mn) ((mn)->mn_Size & ~MN_SIZE_NEVERFREE)

struct MemoryNode
{
	ULONG				mn_Size;
};

struct AVLMemoryNode {
    /* Variables used to track nodes and free them */
    struct AVLNode  amn_AvlNode;
    void           *amn_Address;
    ULONG           amn_Size;
};

/* This keeps track of individual slabs. Each slab begins with this
 * header and is followed by the memory it manages. The size of that
 * memory "slab" is fixed and matches what is stored in
 * SlabData.sd_StandardSlabSize.
 *
 * Each slab manages allocations of a specific maximum size, e.g. 8, 16, 32,
 * 64, etc. bytes. Multiple slabs can exist which manage allocations of the same
 * size, in case one such slab is not enough. Allocations are made from chunks,
 * and for each slab, all chunks are the same size.
 */
struct SlabNode
{
	struct MinNode	sn_MinNode;

	/* If this slab is empty, it goes into a list of slabs to be
	 * purged when memory is tight, or if it has stuck around long
	 * enough without getting purged. This is what the sn_EmptyDecay
	 * field is for. sn_EmptyDecay is decreased whenever an allocation
	 * suceeds which did not use this slab, and when sn_EmptyDecay
	 * reaches 0, the empty slab is purged.
	 */
	struct MinNode	sn_EmptyLink;
	ULONG			sn_EmptyDecay;

	/* How many chunks of memory does this slab contain? */
	ULONG			sn_Count;
	/* How large are the individual chunks? */
	ULONG			sn_ChunkSize;
	/* How many chunks of this slab are currently in use? */
	ULONG			sn_UseCount;

	/* How many times was this slab reused instead of allocating
	 * it from system memory?
	 */
	ULONG			sn_NumReused;

	/* This contains all the chunks of memory which are available
	 * for allocation.
	 */
	struct MinList	sn_FreeList;
};

/* Memory allocations which are not part of a slab are
 * tracked using this data structure.
 */
struct SlabSingleAllocation
{
	struct MinNode	ssa_MinNode;
	ULONG			ssa_Size;
};

/* This is the global bookkeeping information for managing
 * memory allocations from the slab data structure.
 */
struct SlabData
{
	/* This table contains slabs which manage memory chunks
	 * which are a power of 2 bytes in size, e.g. 8, 16, 32,
	 * 64, 128 bytes. Hence, sd_Slabs[3] keeps track of the slabs
	 * which are 8 bytes in size, sd_Slabs[4] is for 16 byte
	 * chunks, etc. The minimum chunk size is 8, which is why
	 * lists 0..2 are not used. Currently, there is an upper limit
	 * of 2^17 bytes per chunk, but you should not be using slab
	 * chunks much larger than 4096 bytes.
	 */
	struct MinList	sd_Slabs[17];

	/* Memory allocations which are larger than the limit
	 * found in the sd_StandardSlabSize field are kept in this list.
	 * They are never associated with a slab.
	 */
	struct MinList	sd_SingleAllocations;

	/* All slabs which currently are empty, i.e. none of their
	 * memory is being used, are registered in this list.
	 * The list linkage uses the SlabNode.sn_EmptyLink field.
	 */
	struct MinList	sd_EmptySlabs;

	/* This is the standard size of a memory allocation which may
	 * be made from a slab that can accommodate it. This number
	 * is initialized from the __slab_max_size global variable,
	 * if > 0, and unless it already is a power of two, it will
	 * be rounded up to the next largest power of two.
	 */
	size_t			sd_StandardSlabSize;

	/* These fields keep track of how many entries there are in
	 * the sd_SingleAllocations list, and how much memory these
	 * allocations occupy.
	 */
	size_t			sd_NumSingleAllocations;
	size_t			sd_TotalSingleAllocationSize;

	/* If this is set to TRUE, then memory allocations will be
	 * be managed through slabs.
	 */
	BOOL			sd_InUse;
};

struct MemalignEntry
{
    struct AVLNode   me_AvlNode;
    void            *me_Aligned;          /* The address that mmemaligned returned.  */
    void            *me_Exact;            /* The address that malloc returned.  */
};

extern struct SlabData NOCOMMON	__slab_data;
extern unsigned long NOCOMMON	__slab_max_size;
extern unsigned long NOCOMMON	__slab_purge_threshold;

/****************************************************************************/

extern void __free_unused_slabs(void);
extern void * __slab_allocate(size_t allocation_size);
extern void __slab_free(void * address,size_t allocation_size);
extern void __slab_init(size_t slab_size);
extern void __slab_exit(void);

/****************************************************************************/

extern struct MinList NOCOMMON		__memory_list;
extern APTR NOCOMMON				__memory_pool;

extern unsigned long NOCOMMON __maximum_memory_allocated;
extern unsigned long NOCOMMON __current_memory_allocated;
extern unsigned long NOCOMMON __maximum_num_memory_chunks_allocated;
extern unsigned long NOCOMMON __current_num_memory_chunks_allocated;

/****************************************************************************/

extern int NOCOMMON __default_threshold_size;
extern int NOCOMMON __default_puddle_size;

extern void __memory_lock(void);
extern void __memory_unlock(void);

extern int32 MemalignAVLNodeComp(struct AVLNode *avlnode1, struct AVLNode *avlnode2);
extern int32 MemalignAVLKeyComp(struct AVLNode *avlnode1, AVLKey key2);

#endif /* _STDLIB_MEMORY_H */
