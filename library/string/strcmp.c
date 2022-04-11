/*
 * $Id: string_strcmp.c,v 1.5 2010-08-20 15:33:36 clib2devs Exp $
*/

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#ifndef _STRING_HEADERS_H
#include "string_headers.h"
#endif /* _STRING_HEADERS_H */

INLINE STATIC int
__strcmp(const char *s1, const char *s2)
{
	int result = 0;

	if (s1 != s2)
	{
		int c1, c2;

		while ((*s1) == (*s2))
		{
			if ((*s1) == '\0')
				break;

			s1++;
			s2++;
		}

		/* The comparison must be performed as if the
		   characters were unsigned characters. */
		c1 = *(unsigned char *)s1;
		c2 = *(unsigned char *)s2;

		result = c1 - c2;
	}

	return result;
}

int 
strcmp(const char *s1, const char *s2)
{
	int result = 0;

	assert(s1 != NULL && s2 != NULL);

	if (s1 == NULL || s2 == NULL)
	{
		__set_errno(EFAULT);
		goto out;
	}

	/* Make sure __global_clib2 has been created */
	if (__global_clib2 != NULL && __global_clib2->optimizedCPUFunctions) {
		switch (__global_clib2->cpufamily) {
			case CPUFAMILY_4XX:
				result = __strcmp440(s1, s2);
				break;
			default:
				result = __strcmp(s1, s2);
		}
	}
	else {
		/* Fallback to standard function */
		result = __strcmp(s1, s2);
	}


out:

	return (result);
}
