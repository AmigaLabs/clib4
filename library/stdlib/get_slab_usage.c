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
__get_slab_usage(__slab_usage_callback callback)
{
	if(__slab_data.sd_InUse)
	{
		struct __slab_usage_information sui;
		const struct SlabNode * sn;
		BOOL stop;
		int i;

		memset(&sui,0,sizeof(sui));

		__memory_lock();

		sui.sui_slab_size						= __slab_data.sd_StandardSlabSize;
		sui.sui_num_single_allocations			= __slab_data.sd_NumSingleAllocations;
		sui.sui_total_single_allocation_size	= __slab_data.sd_TotalSingleAllocationSize;

		for(i = 0 ; i < (int)NUM_ENTRIES(__slab_data.sd_Slabs) ; i++)
		{
			for(sn = (struct SlabNode *)__slab_data.sd_Slabs[i].mlh_Head ;
			    sn->sn_MinNode.mln_Succ != NULL ;
			    sn = (struct SlabNode *)sn->sn_MinNode.mln_Succ)
			{
				if (sn->sn_UseCount == 0)
					sui.sui_num_empty_slabs++;
				else if (sn->sn_Count == sn->sn_UseCount)
					sui.sui_num_full_slabs++;

				sui.sui_num_slabs++;

				sui.sui_total_slab_allocation_size += sizeof(*sn) + __slab_data.sd_StandardSlabSize;
			}
		}

		if(sui.sui_num_slabs > 0)
		{
			for(i = 0, stop = FALSE ; NOT stop && i < (int)NUM_ENTRIES(__slab_data.sd_Slabs) ; i++)
			{
				for(sn = (struct SlabNode *)__slab_data.sd_Slabs[i].mlh_Head ;
				    sn->sn_MinNode.mln_Succ != NULL ;
				    sn = (struct SlabNode *)sn->sn_MinNode.mln_Succ)
				{
					sui.sui_chunk_size		= sn->sn_ChunkSize;
					sui.sui_num_chunks		= sn->sn_Count;
					sui.sui_num_chunks_used	= sn->sn_UseCount;
					sui.sui_num_reused		= sn->sn_NumReused;

					sui.sui_slab_index++;

					if((*callback)(&sui) != 0)
					{
						stop = TRUE;
						break;
					}
				}
			}
		}
		else
		{
			(*callback)(&sui);
		}

		__memory_unlock();
	}
}
