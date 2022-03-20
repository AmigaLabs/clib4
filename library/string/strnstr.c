/*
 * $Id: string_strnstr.c,v 1.0 2021-03-09 12:04:27 clib2devs Exp $
*/

#ifndef _STRING_HEADERS_H
#include "string_headers.h"
#endif /* _STRING_HEADERS_H */

char *
strnstr(const char *src, const char *sub, size_t len)
{
	const char *max = src + len;

	if (*src == '\0')
		return *sub == '\0' ? (char *)src : (char *)NULL;

	while (src < max)
	{
		int i = 0;
		while (1)
		{
			if (sub[i] == '\0')
				return (char *)src;
			if (sub[i] != src[i])
				break;
			i += 1;
		}
		src += 1;
	}
	return (char *)NULL;
}
