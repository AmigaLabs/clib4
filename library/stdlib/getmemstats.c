/*
 * $Id: stdlib_getmemstats.c,v 1.2 2006-01-08 12:04:25 clib2devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

void
__get_mem_stats(size_t * current_memory,size_t * max_memory,size_t * current_chunks,size_t * max_chunks)
{
	__memory_lock();

	if(current_memory != NULL)
		(*current_memory) = __current_memory_allocated;

	if(max_memory != NULL)
		(*max_memory) = __maximum_memory_allocated;

	if(current_chunks != NULL)
		(*current_chunks) = __current_num_memory_chunks_allocated;

	if(max_chunks != NULL)
		(*max_chunks) = __maximum_num_memory_chunks_allocated;

	__memory_unlock();
}
