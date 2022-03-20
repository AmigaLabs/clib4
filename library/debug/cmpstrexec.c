/*
 * $Id: debug_cmpstrexec.c,v 1.3 2006-01-08 12:04:22 clib2devs Exp $
*/

#include "debug_headers.h"

/****************************************************************************/

LONG
cmpstrexec(const char * source1,const char * source2)
{
	LONG result = 0;

	assert( source1 != NULL && source2 != NULL );

	if(source1 != NULL && source2 != NULL)
	{
		while((*source1) == (*source2))
		{
			if((*source1) == '\0')
				goto out;

			source1++;
			source2++;
		}

		result = (*source1) - (*source2);
	}

 out:

	return(result);
}
