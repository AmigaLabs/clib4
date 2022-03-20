/*
 * $Id: stdio_ftello.c,v 1.2 2006-01-08 12:04:24 clib2devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

/* ftello() is technically equivalent to ftell(), except that
   the return value is of type "off_t" rather than "long int".
   In this implementation, "off_t" has the same range as the
   "long int" type, so this matters little. */
off_t
ftello(FILE *stream)
{
	off_t result;

	result = (off_t)ftell(stream);

	return(result);
}
