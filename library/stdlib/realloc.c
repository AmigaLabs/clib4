/*
 * $Id: stdlib_realloc.c,v 1.10 2006-02-17 10:55:03 clib2devs Exp $
*/

/*#define DEBUG*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _STDLIB_MEMORY_H
#include "stdlib_memory.h"
#endif /* _STDLIB_MEMORY_H */

void *realloc(void *ptr, size_t size)
{
	void *result = NULL;
	BOOL locked = FALSE;

	ENTER();

	SHOWPOINTER(ptr);
	SHOWVALUE(size);

	assert((int)size >= 0);

	if (ptr == NULL)
	{
		D(("calling malloc(%ld)", size));

		result = malloc(size);
	}
	else if (__unix_path_semantics && size == 0)
	{
		D(("calling free(0x%08lx)", ptr));

		free(ptr);
	}
	else
	{
        assert(ptr != NULL);

		size_t old_size;
		BOOL reallocate;

        __memory_lock();
        locked = TRUE;

        struct MemoryNode *mn;


		/* Try to find the allocation in the list. */
		mn = __find_memory_node(ptr);

		if (mn == NULL || FLAG_IS_SET(mn->mn_Size, MN_SIZE_NEVERFREE))
		{
			SHOWMSG("cannot free this chunk");
			goto out;
		}

		old_size = GET_MN_SIZE(mn);

        /* Don't do anything unless the size of the allocation has really changed. */
        if (size > old_size)
        {
            /* Allocation size should grow. */
            reallocate = TRUE;
        }
        else
        {
            /* Optimization: If the block size shrinks by less than half the
                             original allocation size, do not reallocate the
                             block and do not copy over the contents of the old
                             allocation. We also take into account that the
                             actual size of the allocation is affected by a
                             certain operating system imposed granularity. */
            reallocate = (size < old_size && size <= old_size / 2);
        }

		if (reallocate)
		{
			void *new_ptr;

			D(("realloc() size has changed; old=%ld, new=%ld", old_size, size));

			/* We allocate the new memory chunk before we
			   attempt to replace the old. */
			new_ptr = malloc(size);
			if (new_ptr == NULL)
			{
				SHOWMSG("could not reallocate memory");
				goto out;
			}

			/* Copy the contents of the old allocation to the new buffer. */
			if (size > old_size)
				size = old_size;

			memmove(new_ptr, ptr, size);

			/* Free the old allocation. Since we already know which memory
			   node is associated with it, we don't call __free() here. */
			__free_memory_node(mn);
			result = new_ptr;
		}
		else
		{
			D(("size didn't actually change that much (%ld -> %ld); returning memory block as is.", old_size, size));

			/* No change in size. */
			result = ptr;
		}
	}

out:

	if (locked)
		__memory_unlock();

	if (result == NULL)
		SHOWMSG("ouch! realloc failed");

	RETURN(result);
	return (result);
}
