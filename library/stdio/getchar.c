/*
 * $Id: stdio_getchar.c,v 1.3 2006-01-08 12:04:24 clib2devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

/****************************************************************************/

#undef getchar

/****************************************************************************/

int
getchar(void)
{
	int result;

	result = fgetc(stdin);

	return(result);
}
