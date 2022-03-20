/*
 * $Id: stdlib_malloc.c,v 1.21 2021-01-31 14:09:00 clib2devs Exp $
*/

/*#define DEBUG*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _STDLIB_MEMORY_H
#include "stdlib_memory.h"
#endif /* _STDLIB_MEMORY_H */

#ifndef _STDLIB_CONSTRUCTOR_H
#include "stdlib_constructor.h"
#endif /* _STDLIB_CONSTRUCTOR_H */

#undef malloc

/****************************************************************************/

unsigned long NOCOMMON __maximum_memory_allocated;
unsigned long NOCOMMON __current_memory_allocated;
unsigned long NOCOMMON __maximum_num_memory_chunks_allocated;
unsigned long NOCOMMON __current_num_memory_chunks_allocated;

APTR NOCOMMON __memory_pool;
struct MinList NOCOMMON __memory_list;

void *
__allocate_memory(size_t size, BOOL never_free)
{
	struct MemoryNode *mn;
	size_t allocation_size;
	void *result = NULL;

#if defined(UNIX_PATH_SEMANTICS)
	size_t original_size = -1;

	if (__global_clib2 != NULL && __global_clib2->__unix_path_semantics)
	{
        original_size = size;
		/* The libunix.a flavour accepts zero length memory allocations
		   and quietly turns them into a pointer sized allocations. */
		if (size == 0) {
			size = sizeof(char *);
        }
	}
#endif /* UNIX_PATH_SEMANTICS */

	__memory_lock();

	/* Zero length allocations are by default rejected. */
	if (size == 0)
	{
		__set_errno(EINVAL);
		goto out;
	}

	if (__free_memory_threshold > 0 && AvailMem(MEMF_ANY | MEMF_LARGEST) < __free_memory_threshold)
	{
		SHOWMSG("not enough free memory available to safely proceed with allocation");
		goto out;
	}

    /* Round up allocation to a multiple of 32 bits. */
    if ((size & 3) != 0)
        size += 4 - (size & 3);

    allocation_size = sizeof(*mn) + size;

	/* Integer overflow has occured? */
	if (size == 0 || allocation_size < size)
	{
		__set_errno(ENOMEM);
		goto out;
	}

	/* We reuse the MemoryNode.mn_Size field to mark
	 * allocations are not suitable for use with
	 * free() and realloc(). This limits allocation
	 * sizes to a little less than 2 GBytes.
	 */
	if (allocation_size & MN_SIZE_NEVERFREE)
	{
		__set_errno(ENOMEM);
		goto out;
	}

    /* Are we using the slab allocator? */
    if (__slab_data.sd_InUse)
    {
        mn = __slab_allocate(allocation_size);
    }
    else
    {
        if (__memory_pool != NULL)
        {
            mn = AllocPooled(__memory_pool, allocation_size);
        }
        else
        {
            struct MinNode *mln;

            mln = AllocVecTags(sizeof(*mln) + allocation_size, AVT_Type, MEMF_PRIVATE, TAG_DONE);
            if (mln != NULL)
            {
                AddTail((struct List *)&__memory_list, (struct Node *)mln);
                mn = (struct MemoryNode *)&mln[1];
            }
            else
            {
                mn = NULL;
            }
        }
    }

	if (mn == NULL)
	{
		SHOWMSG("not enough memory");
		goto out;
	}

	mn->mn_Size = size;

	if (never_free)
		SET_FLAG(mn->mn_Size, MN_SIZE_NEVERFREE);

	__current_memory_allocated += allocation_size;
	if (__maximum_memory_allocated < __current_memory_allocated)
		__maximum_memory_allocated = __current_memory_allocated;

	__current_num_memory_chunks_allocated++;
	if (__maximum_num_memory_chunks_allocated < __current_num_memory_chunks_allocated)
		__maximum_num_memory_chunks_allocated = __current_num_memory_chunks_allocated;

    result = &mn[1];

#if defined(UNIX_PATH_SEMANTICS)
	if (__global_clib2->__unix_path_semantics)
	{
		/* Set the zero length allocation contents to NULL. */
		if (original_size == 0)
			*(char **)result = NULL;
	}
#endif /* UNIX_PATH_SEMANTICS */

	assert((((ULONG)result) & 3) == 0);

out:

	__memory_unlock();

	return (result);
}

void * malloc(size_t size)
{
	void *result = NULL;

	__memory_lock();

	/* Try to get rid of now unused memory. */
	if (__alloca_cleanup != NULL)
		(*__alloca_cleanup)();

	__memory_unlock();

	/* Allocate memory which can be put through realloc() and free(). */
	result = __allocate_memory(size, FALSE);

	return (result);
}

static struct SignalSemaphore *memory_semaphore;

void __memory_lock(void)
{
	if (memory_semaphore != NULL)
		ObtainSemaphore(memory_semaphore);
}

void __memory_unlock(void)
{
	if (memory_semaphore != NULL)
		ReleaseSemaphore(memory_semaphore);
}

STDLIB_DESTRUCTOR(stdlib_memory_exit)
{
	ENTER();

    /* Is the slab memory allocator enabled? */
    if (__slab_data.sd_InUse)
    {
        __slab_exit();
    }
    else
    {
        __memory_lock();
        if (__memory_pool != NULL)
        {
            NewList((struct List *)&__memory_list);

            FreeSysObject(ASOT_MEMPOOL, __memory_pool);
            __memory_pool = NULL;
        }
        else if (__memory_list.mlh_Head != NULL)
        {
            while (NOT IsMinListEmpty(&__memory_list))
                __free_memory_node((struct MemoryNode *)__memory_list.mlh_Head);
        }
        __memory_unlock();
    }

	__delete_semaphore(memory_semaphore);
	memory_semaphore = NULL;

	LEAVE();
}

/* Second constructor called by _init */
STDLIB_CONSTRUCTOR(stdlib_memory_init)
{
	BOOL success = FALSE;

	ENTER();

	memory_semaphore = __create_semaphore();
	if (memory_semaphore == NULL)
		goto out;

	NewList((struct List *)&__memory_list);

    /* Enable the slab memory allocator? */
    if (__slab_max_size > 0)
    {
        __slab_init(__slab_max_size);
    }
    else
    {
        __memory_pool = AllocSysObjectTags(ASOT_MEMPOOL,
                                           ASOPOOL_MFlags, MEMF_PRIVATE,
                                           ASOPOOL_Threshold, (ULONG)__default_threshold_size,
                                           ASOPOOL_Puddle, (ULONG)__default_puddle_size,
                                           TAG_DONE);
        if (!__memory_pool) {
            goto out;
        }
    }

	success = TRUE;

out:

	SHOWVALUE(success);
	LEAVE();

	if (success)
		CONSTRUCTOR_SUCCEED();
	else
		CONSTRUCTOR_FAIL();
}
