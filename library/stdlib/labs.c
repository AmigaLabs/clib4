/*
 * $Id: stdlib_labs.c,v 1.3 2006-01-08 12:04:25 clib2devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

long
labs(long x)
{
	long result;

    ENTER();
    SHOWVALUE(x);

    result = (x < 0) ? (-x) : x;

    RETURN(result);
	return(result);
}
