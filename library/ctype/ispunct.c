/*
 * $Id: ctype_ispunct.c,v 1.5 2006-01-08 12:04:22 clib2devs Exp $
*/

#ifndef _CTYPE_HEADERS_H
#include "ctype_headers.h"
#endif /* _CTYPE_HEADERS_H */

/****************************************************************************/

#undef ispunct

/****************************************************************************/

int
ispunct(int c)
{
	DECLARE_LOCALEBASE();
	int result;

	__locale_lock();

	if(__locale_table[LC_CTYPE] != NULL)
	{
		assert( LocaleBase != NULL );

		/* The parameter must be either EOF or in the range of an
		   'unsigned char'. If it's not, then the behaviour is
		   undefined. */
		if(c != EOF && ((0 <= c && c <= UCHAR_MAX) || ((c + 256) <= UCHAR_MAX)))
			result = IsPunct(__locale_table[LC_CTYPE],(ULONG)(c & 255));
		else
			result = FALSE;
	}
	else
	{
		result = (('!' <= c && c <= '/') ||
		          (':' <= c && c <= '@') ||
		          ('[' <= c && c <= '`') ||
		          ('{' <= c && c <= '~'));
	}

	__locale_unlock();

	return(result);
}
