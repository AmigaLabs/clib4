/*
 * $Id: wchar_wcrtomb.c,v 1.4 2021-02-03 18:48:27 apalmate Exp $
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

/****************************************************************************/

size_t
wcrtomb(char *s, wchar_t wc, mbstate_t *st)
{
	(void)st;
	if (!s)
		return 1;
	if ((unsigned)wc < 0x80U)
	{
		*s = (char)wc;
		return 1;
	}
	else if ((unsigned)wc < 0x800U)
	{
		*s++ = (char)(0xc0 | (wc >> 6));
		*s = (char)(0x80 | (wc & 0x3f));
		return 2;
	}
	else if (((unsigned)wc < 0xd800U) || (((unsigned)wc - 0xe000U) < 0x2000U))
	{
		*s++ = (char)(0xe0 | (wc >> 12));
		*s++ = (char)(0x80 | ((wc >> 6) & 0x3f));
		*s = (char)(0x80 | (wc & 0x3f));
		return 3;
	}
	else if (((unsigned)wc - 0x10000U) < 0x100000U)
	{
		*s++ = (char)(0xf0 | (wc >> 18));
		*s++ = (char)(0x80 | ((wc >> 12) & 0x3f));
		*s++ = (char)(0x80 | ((wc >> 6) & 0x3f));
		*s = (char)(0x80 | (wc & 0x3f));
		return 4;
	}
	errno = EILSEQ;
	return (size_t)-1;
}
