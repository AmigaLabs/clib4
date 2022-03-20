/*
 * $Id: stdlib_atof.c,v 1.5 2021-03-29 16:34:25 clib2devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

double
atof(const char *str)
{
	double result = 0.0;

	assert(str != NULL);

	if (str == NULL)
	{
		__set_errno(EFAULT);
		goto out;
	}

	result = strtod(str, NULL);

out:

	return (result);
}
