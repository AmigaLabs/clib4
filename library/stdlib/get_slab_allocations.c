/*

 */

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

/****************************************************************************/

#ifndef _STDLIB_MEMORY_H
#include "stdlib_memory.h"
#endif /* _STDLIB_MEMORY_H */

/****************************************************************************/

void
__get_slab_allocations(__slab_allocation_callback callback)
{
	if(__slab_data.sd_InUse)
	{
		struct __slab_allocation_information sai;

		memset(&sai,0,sizeof(sai));

		__memory_lock();

		sai.sai_num_single_allocations			= __slab_data.sd_NumSingleAllocations;
		sai.sai_total_single_allocation_size	= __slab_data.sd_TotalSingleAllocationSize;

		if(__slab_data.sd_SingleAllocations.mlh_Head->mln_Succ != NULL)
		{
			const struct SlabSingleAllocation * ssa;

			for(ssa = (struct SlabSingleAllocation *)__slab_data.sd_SingleAllocations.mlh_Head ;
			    ssa->ssa_MinNode.mln_Succ != NULL ;
			    ssa = (struct SlabSingleAllocation *)ssa->ssa_MinNode.mln_Succ)
			{
				sai.sai_allocation_index++;

				sai.sai_allocation_size			= ssa->ssa_Size - sizeof(*ssa);
				sai.sai_total_allocation_size	= ssa->ssa_Size;

				if((*callback)(&sai) != 0)
					break;
			}
		}
		else
		{
			(*callback)(&sai);
		}

		__memory_unlock();
	}
}
