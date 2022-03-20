/*
 * $Id: stdlib_free.c,v 1.14 2031-01-31 12:04:25 clib2devs Exp $
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

struct MemoryNode *
__find_memory_node(void *address)
{
	struct MemoryNode *result;

	assert(address != NULL);

	result = &((struct MemoryNode *)address)[-1];

	return (result);
}

STATIC VOID
remove_and_free_memory_node(struct MemoryNode *mn)
{
	if (mn == NULL)
		return;
		
	size_t allocation_size;

	assert(mn != NULL);

	__memory_lock();

    allocation_size = sizeof(*mn) + GET_MN_SIZE(mn);

    /* Are we using the slab allocator? */
    if (__slab_data.sd_InUse)
    {
        __slab_free(mn, allocation_size);
    }
    else
    {
        if (__memory_pool != NULL)
        {
            if (mn != NULL) {
                FreeVecPooled(__memory_pool, mn);
                mn = NULL;
            }
        }
        else
        {
            struct MinNode *mln = (struct MinNode *)mn;

            mln--;

            Remove((struct Node *)mln);

            FreeVec(mln);
            mln = NULL;
        }
    }

	__current_memory_allocated -= allocation_size;
	__current_num_memory_chunks_allocated--;

	__memory_unlock();
}

void __free_memory_node(struct MemoryNode *mn)
{
	assert(mn != NULL);

    remove_and_free_memory_node(mn);
}

VOID __free_memory(void *ptr, BOOL force)
{
    BOOL found = FALSE;

	struct MemoryNode *mn;

	assert(ptr != NULL);

	SHOWPOINTER(ptr);
	SHOWVALUE(force);

	/* Check if we have something created by memalign */
    if (__global_clib2 != NULL) {
        struct MemalignEntry *e = (struct MemalignEntry *) AVL_FindNode(__global_clib2->__memalign_tree, ptr,
                                                                        MemalignAVLKeyComp);
        if (e) {
            mn = __find_memory_node(e->me_Exact);
            AVL_RemNodeByAddress(&__global_clib2->__memalign_tree, &e->me_AvlNode);
            ItemPoolFree(__global_clib2->__memalign_pool, e);
            found = TRUE;
        }
    }
	if (!found) {
		mn = __find_memory_node(ptr);
	}

    assert(mn != NULL);

    SHOWVALUE(mn->mn_Size);

    if (mn != NULL && (force || FLAG_IS_CLEAR(mn->mn_Size, MN_SIZE_NEVERFREE)))
        __free_memory_node(mn);
}

void free(void *ptr)
{
	__memory_lock();

	/* Try to get rid of now unused memory. */
	if (__alloca_cleanup != NULL)
		(*__alloca_cleanup)();

	__memory_unlock();

	if (ptr != NULL)
		__free_memory(ptr, FALSE);
}
