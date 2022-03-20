/*
 * $Id: stdlib_resetmemstats.c,v 1.2 2006-01-08 12:04:26 clib2devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

/****************************************************************************/

void
__reset_max_mem_stats(void)
{
	__memory_lock();

	__maximum_memory_allocated				= __current_memory_allocated;
	__maximum_num_memory_chunks_allocated	= __current_num_memory_chunks_allocated;

	__memory_unlock();
}
