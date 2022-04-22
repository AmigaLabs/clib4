/*
 * $Id: stdlib_atoll.c,v 1.3 2006-08-02 08:00:27 clib2devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

long long
atoll(const char *str)
{
	long long result = 0;

    ENTER();
    SHOWSTRING(str);

    assert( str != NULL );

		if(str == NULL)
		{
			__set_errno(EFAULT);
			goto out;
		}

	result = strtoll(str, (char **)NULL, 10);

 out:
    RETURN(result);
	return(result);
}
