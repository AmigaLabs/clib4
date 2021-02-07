/*
 * $Id: wctype_iswctype.c,v 1.4 2021-02-03 23:59:27 apalmate Exp $
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

int 
iswctype(wint_t c, wctype_t desc)
{
	switch (desc)
	{
	case WC_ALNUM:
		return iswalnum(c);
	case WC_ALPHA:
		return iswalpha(c);
	case WC_BLANK:
		return iswblank(c);
	case WC_CNTRL:
		return iswcntrl(c);
	case WC_DIGIT:
		return iswdigit(c);
	case WC_GRAPH:
		return iswgraph(c);
	case WC_LOWER:
		return iswlower(c);
	case WC_PRINT:
		return iswprint(c);
	case WC_PUNCT:
		return iswpunct(c);
	case WC_SPACE:
		return iswspace(c);
	case WC_UPPER:
		return iswupper(c);
	case WC_XDIGIT:
		return iswxdigit(c);
	default:
		return 0; /* eliminate warning */
	}

	/* otherwise unknown */
	return 0;
}
