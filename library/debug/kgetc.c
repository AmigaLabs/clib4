/*
 * $Id: debug_kgetc.c,v 1.3 2006-01-08 12:04:22 clib2devs Exp $
*/

#include "debug_headers.h"

/****************************************************************************/

LONG
kgetc(VOID)
{
	LONG result;

	do
		result = KMayGetChar();
	while(result < 0);

	return(result);
}
