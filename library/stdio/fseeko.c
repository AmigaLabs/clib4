/*
 * $Id: stdio_fseeko.c,v 1.2 2006-01-08 12:04:24 clib2devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

/* fseeko() is technically equivalent to fseek(), except that the
   "offset" parameter is of type "off_t" rather than "long int".
   In this implementation, "off_t" has the same range as the
   "long int" type, so this matters little. */
int
fseeko(FILE *stream, off_t offset, int wherefrom)
{
	int result;

	result = fseek(stream,(long int)offset,wherefrom);

	return(result);
}
