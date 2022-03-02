/*
 * $Id: wctype_wctype.c,v 1.4 2021-02-04 00:25:27 apalmate Exp $
 *
 * :ts=4
 *
 * Portable ISO 'C' (1994) runtime library for the Amiga computer
 * Copyright (c) 2002-2015 by Olaf Barthel <obarthel (at) gmx.net>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   - Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *
 *   - Neither the name of Olaf Barthel nor the names of contributors
 *     may be used to endorse or promote products derived from this
 *     software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _WCHAR_HEADERS_H
#include "wchar_headers.h"
#endif /* _WCHAR_HEADERS_H */

#ifndef _WCTYPE_HEADERS_H
#include <wctype.h>
#endif /* _WCTYPE_HEADERS_H */

wctype_t
wctype(const char *property)
{
	switch (*property)
	{
	case 'a':
		if (!strcmp(property, "alnum"))
			return WC_ALNUM;
		else if (!strcmp(property, "alpha"))
			return WC_ALPHA;
		break;
	case 'b':
		if (!strcmp(property, "blank"))
			return WC_BLANK;
		break;
	case 'c':
		if (!strcmp(property, "cntrl"))
			return WC_CNTRL;
		break;
	case 'd':
		if (!strcmp(property, "digit"))
			return WC_DIGIT;
		break;
	case 'g':
		if (!strcmp(property, "graph"))
			return WC_GRAPH;
		break;
	case 'l':
		if (!strcmp(property, "lower"))
			return WC_LOWER;
		break;
	case 'p':
		if (!strcmp(property, "print"))
			return WC_PRINT;
		else if (!strcmp(property, "punct"))
			return WC_PUNCT;
		break;
	case 's':
		if (!strcmp(property, "space"))
			return WC_SPACE;
		break;
	case 'u':
		if (!strcmp(property, "upper"))
			return WC_UPPER;
		break;
	case 'x':
		if (!strcmp(property, "xdigit"))
			return WC_XDIGIT;
		break;
	}

	/* otherwise invalid */
	__set_errno(EINVAL);
	return 0;
}
