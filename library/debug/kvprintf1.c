/*
 * $Id: debug_kvprintf1.c,v 1.3 2006-01-08 12:04:22 clib2devs Exp $
*/

#include "debug_headers.h"

VOID
kvprintf(const char * format,va_list arg)
{
	assert( format != NULL );

	if(format != NULL)
		KPutFmt(format,arg);
}
