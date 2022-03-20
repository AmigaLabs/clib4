/*
 * $Id: stdlib_alloca.c,v 1.11 2006-01-08 12:04:25 clib2devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

/****************************************************************************/

#ifndef _STDLIB_MEMORY_H
#include "stdlib_memory.h"
#endif /* _STDLIB_MEMORY_H */

/****************************************************************************/

#ifndef _STDLIB_CONSTRUCTOR_H
#include "stdlib_constructor.h"
#endif /* _STDLIB_CONSTRUCTOR_H */

/****************************************************************************/

#undef alloca

extern void *alloca(size_t size);

/****************************************************************************/

static struct MinList alloca_memory_list;

/****************************************************************************/

struct MemoryContextNode
{
	struct MinNode mcn_MinNode; /* The usual linkage. */
	void *mcn_StackPointer;		/* Points to stack frame this allocation
										   is associated with. */
	void *mcn_Memory;			/* Points to the memory allocated. */
};

/****************************************************************************/

CLIB_DESTRUCTOR(alloca_exit)
{
	ENTER();

	/* Clean this up, too, just to be safe. */
	NewList((struct List *)&alloca_memory_list);

	LEAVE();
}

/****************************************************************************/

/* Cleans up after all alloca() allocations that have been made so far. */
static void
alloca_cleanup()
{
	void *stack_pointer = __get_sp();

	__memory_lock();

	/* Initialize this if it hasn't been taken care of yet. */
	if (alloca_memory_list.mlh_Head == NULL)
		NewList((struct List *)&alloca_memory_list);

	/* Is this worth cleaning up? */
	if (NOT IsMinListEmpty(&alloca_memory_list))
	{
		struct MemoryContextNode *mcn_prev;
		struct MemoryContextNode *mcn;

		/* The assumption is that the stack grows downwards. If this function is
		   called, we must get rid off all the allocations associated with stack
		   pointers whose addresses are smaller than the current stack pointer.
		   Which so happen to be stored near the end of the list. The further
		   we move up from the end to the top of the list, the closer we get
		   to the allocations made in the context of a stack frame near to
		   where were currently are. */
		for (mcn = (struct MemoryContextNode *)alloca_memory_list.mlh_TailPred;
			 mcn->mcn_MinNode.mln_Pred != NULL && mcn->mcn_StackPointer < stack_pointer;
			 mcn = mcn_prev)
		{
			mcn_prev = (struct MemoryContextNode *)mcn->mcn_MinNode.mln_Pred;

			Remove((struct Node *)mcn);

			__free_memory(mcn->mcn_Memory, TRUE);
			__free_memory(mcn, FALSE);
		}

		/* Drop the cleanup callback if there's nothing to be cleaned
		   up any more. */
		if (IsMinListEmpty(&alloca_memory_list))
			__alloca_cleanup = NULL;
	}

	__memory_unlock();
}

/****************************************************************************/

void *alloca(size_t size)
{
	void *stack_pointer = __get_sp();
	struct MemoryContextNode *mcn;
	void *result = NULL;

	__memory_lock();

	/* Initialize this if it hasn't been taken care of yet. */
	if (alloca_memory_list.mlh_Head == NULL)
		NewList((struct List *)&alloca_memory_list);

	__alloca_cleanup = alloca_cleanup;
	(*__alloca_cleanup)();

	mcn = __allocate_memory(sizeof(*mcn), FALSE);
	if (mcn == NULL)
	{
		SHOWMSG("not enough memory");
		goto out;
	}

	/* Allocate memory which cannot be run through realloc() or free(). */
	mcn->mcn_Memory = __allocate_memory(size, TRUE);
	if (mcn->mcn_Memory == NULL)
	{
		SHOWMSG("not enough memory");

		free(mcn);
		goto out;
	}

	mcn->mcn_StackPointer = stack_pointer;

	assert(alloca_memory_list.mlh_Head != NULL);

	AddTail((struct List *)&alloca_memory_list, (struct Node *)mcn);

	result = mcn->mcn_Memory;

out:

	__memory_unlock();

	/* If we are about to return NULL and a trap function is
	   provided, call it rather than returning NULL. */
	if (result == NULL && __alloca_trap != NULL)
		(*__alloca_trap)();

	return (result);
}
