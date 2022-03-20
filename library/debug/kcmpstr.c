/*
 * $Id: debug_kcmpstr.c,v 1.3 2006-01-08 12:04:22 clib2devs Exp $
*/

#include "debug_headers.h"

/****************************************************************************/

LONG
KCmpStr(const char * source1,const char * source2)
{
	LONG result;

	result = cmpstrexec(source1,source2);

	return(result);
}
