/*
 * $Id: wchar_fputws.c,v 1.3 2006-01-08 12:04:27 clib2devs Exp $
*/

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#ifndef _WCHAR_HEADERS_H
#include "wchar_headers.h"
#endif /* _WCHAR_HEADERS_H */

wint_t
fputws(const wchar_t *ws, FILE *fp)
{
	char buf[BUFSIZ] = {0};
	int nwritten = 0;
	int nchars = (int)wcslen(ws);

	while (nchars > 0)
	{
		int nbytes = 0;
		char *ptr = (char *)&buf;
		while ((nbytes < (BUFSIZ - (MB_LEN_MAX * 2))) && (nchars))
		{
			int n = wctomb(ptr, *ws);
			if (n < 0)
			{
				__set_errno(EILSEQ);
				return 0;
			}
			ws++;
			ptr += n;
			nbytes += n;
			nchars--;
		}
		*ptr = '\0';
		if (fputs(buf, fp) < nbytes)
		{
			return 0;
		}
		nwritten += nbytes;
	}
	return (nwritten);
}
