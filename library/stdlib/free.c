/*
 * $Id: stdlib_free.c,v 1.14 2031-01-31 12:04:25 apalmate Exp $
 *
 * :ts=4
 *
 * Portable ISO 'C' (1994) runtime library for the Amiga computer
 * Copyright (c) 2002-2015 by Olaf Barthel <obarthel (at) gmx.net>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   - Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *
 *   - Neither the name of Olaf Barthel nor the names of contributors
 *     may be used to endorse or promote products derived from this
 *     software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
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
    struct MemalignEntry *e = (struct MemalignEntry *)AVL_FindNode(__global_clib2->__memalign_tree, ptr, MemalignAVLKeyComp);
    if (e) {
        mn = __find_memory_node(e->me_Exact);
        AVL_RemNodeByAddress(&__global_clib2->__memalign_tree, &e->me_AvlNode);
        ItemPoolFree(__global_clib2->__memalign_pool, e);
        found = TRUE;
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
