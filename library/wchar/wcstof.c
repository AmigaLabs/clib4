/*
 * $Id: wchar_wcstof.c,v 1.0 2021-09-28 12:04:27 clib2devs Exp $
*/

#ifndef _WCHAR_HEADERS_H
#include "wchar_headers.h"
#endif /* _WCHAR_HEADERS_H */

#ifndef _WCTYPE_HEADERS_H
#include <wctype.h>
#endif /* _WCTYPE_HEADERS_H */

#ifndef _MATH_HEADERS_H
#include "math_headers.h"
#endif /* _MATH_HEADERS_H */

float
wcstof(const wchar_t *nptr, wchar_t **endptr)
{
	double val = wcstod(nptr, endptr);
	if (__stdlib__isnan(val))
		return __stdlib_nanf("");
	float retval = (float)val;
	if (__stdlib__isinf(retval) && !__stdlib__isinf(val))
		__set_errno(ERANGE);

	return retval;
}
