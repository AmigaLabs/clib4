/*
 * $Id: string_strtok.c,v 1.6 2006-01-08 12:04:27 clib4devs Exp $
*/

#ifndef _STRING_HEADERS_H
#include "string_headers.h"
#endif /* _STRING_HEADERS_H */

char *
strtok(char *str, const char *separator_set)
{
	static char * last;

	char * result;

	ENTER();

	result = strtok_r(str,separator_set,&last);
	
	RETURN(result);
	return(result);
}
