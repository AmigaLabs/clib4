/*
 * $Id: string_rindex.c,v 1.4 2006-01-08 12:04:26 clib2devs Exp $
*/

#ifndef _STRING_HEADERS_H
#include "string_headers.h"
#endif /* _STRING_HEADERS_H */

char *
rindex(const char *s, int c)
{
	char * result;

	result = strrchr(s,c);

	return(result);
}
