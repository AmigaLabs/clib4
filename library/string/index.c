/*
 * $Id: string_index.c,v 1.4 2006-01-08 12:04:26 clib2devs Exp $
*/

#ifndef _STRING_HEADERS_H
#include "string_headers.h"
#endif /* _STRING_HEADERS_H */

char *
index(const char *s, int c)
{
	char * result;

	result = strchr(s,c);

	return(result);
}
