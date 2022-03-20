/*
 * $Id: string_bcopy.c,v 1.4 2006-01-08 12:04:26 clib2devs Exp $
*/

#ifndef _STRING_HEADERS_H
#include "string_headers.h"
#endif /* _STRING_HEADERS_H */

void
bcopy(const void *src,void *dest,size_t len)
{
	assert( (len == 0) || (src != NULL && dest != NULL && (int)len > 0) );

	memmove(dest,src,len);
}
