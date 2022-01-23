/*
 * $Id: wctype_iswcntrl.c,v 1.4 2021-02-03 23:58:27 apalmate Exp $
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

#include <ctype.h>

/****************************************************************************/

int 
iswcntrl(wint_t c)
{
	int unicode = 0;
	if (__global_clib2->_current_locale[0] == 'C' && __global_clib2->_current_locale[1] == '\0')
	{
		unicode = 0;
		/* fall-through */
	}
	else if (!strcmp(__global_clib2->_current_locale, "C-JIS"))
	{
		c = __jp2uc(c, JP_JIS);
		unicode = 1;
	}
	else if (!strcmp(__global_clib2->_current_locale, "C-SJIS"))
	{
		c = __jp2uc(c, JP_SJIS);
		unicode = 1;
	}
	else if (!strcmp(__global_clib2->_current_locale, "C-EUCJP"))
	{
		c = __jp2uc(c, JP_EUCJP);
		unicode = 1;
	}
	else if (!strcmp(__global_clib2->_current_locale, "C-UTF-8"))
	{
		unicode = 1;
	}

	if (1) //(unicode)
	{
		return ((c >= 0x0000 && c <= 0x001f) ||
				(c >= 0x007f && c <= 0x009f) ||
				c == 0x2028 || c == 0x2029);
	}

	return (c < 0x100 ? iscntrl(c) : 0);
}
