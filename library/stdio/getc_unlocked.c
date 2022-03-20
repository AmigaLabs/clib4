/*
 * $Id: stdio_getc_unlocked.c,v 1.3 2006-01-08 12:04:24 clib2devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#undef getc_unlocked

int
getc_unlocked(FILE *stream)
{
	int result = EOF;

	assert( stream != NULL );

    if(stream == NULL)
    {
        __set_errno(EFAULT);
        goto out;
    }

	result = __getc_unlocked(stream);

 out:

	return(result);
}
