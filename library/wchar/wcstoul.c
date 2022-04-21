/*
 * $Id: wchar_wcstoul.c,v 1.3 2006-01-08 12:04:27 clib2devs Exp $
*/

#ifndef _WCHAR_HEADERS_H
#include "wchar_headers.h"
#endif /* _WCHAR_HEADERS_H */

#include <wctype.h>

unsigned long
wcstoul(const wchar_t *nptr, wchar_t **endptr, int base)
{
	register const wchar_t *s = nptr;
	register unsigned long acc;
	register int c;
	register unsigned long cutoff;
	register int neg = 0, any, cutlim;

    ENTER();

    /*
     * See strtol for comments as to the logic used.
     */
	do
	{
		c = *s++;
	} while (iswspace(c));
	if (c == L'-')
	{
		neg = 1;
		c = *s++;
	}
	else if (c == L'+')
		c = *s++;
	if ((base == 0 || base == 16) &&
		c == L'0' && (*s == L'x' || *s == L'X'))
	{
		c = s[1];
		s += 2;
		base = 16;
	}
	if (base == 0)
		base = c == L'0' ? 8 : 10;
	cutoff = (unsigned long)ULONG_MAX / (unsigned long)base;
	cutlim = (unsigned long)ULONG_MAX % (unsigned long)base;
	for (acc = 0, any = 0;; c = *s++)
	{
		if (c >= L'0' && c <= L'9')
			c -= L'0';
		else if (c >= L'A' && c <= L'Z')
			c -= L'A' - 10;
		else if (c >= L'a' && c <= L'z')
			c -= L'a' - 10;
		else
			break;
		if (c >= base)
			break;
		if (any < 0 || acc > cutoff || (acc == cutoff && c > cutlim))
			any = -1;
		else
		{
			any = 1;
			acc *= base;
			acc += c;
		}
	}
	if (any < 0)
	{
		acc = ULONG_MAX;
		__set_errno(ERANGE);
	}
	else if (neg)
		acc = -acc;
	if (endptr != 0)
		*endptr = (wchar_t *)(any ? s - 1 : nptr);
	return (acc);
}
