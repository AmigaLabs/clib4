/*
 * $Id: string_bzero.c,v 1.4 2006-01-08 12:04:26 clib2devs Exp $
*/

#ifndef _STRING_HEADERS_H
#include "string_headers.h"
#endif /* _STRING_HEADERS_H */

void
bzero(void *m,size_t len)
{
	assert( (len == 0) || (m != NULL && (int)len > 0) );

	memset(m,0,len);
}
