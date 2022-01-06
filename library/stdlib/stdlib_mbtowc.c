/*
 * $Id: stdlib_mbtowc.c,v 1.3 2006-01-08 12:04:26 obarthel Exp $
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

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#include "wchar_headers.h"

int 
mbtowc(wchar_t *restrict wc, const char *restrict src, size_t n)
{
#ifdef LIBWCHAR
	unsigned c;
	const unsigned char *s = (const void *)src;

	if (!s)
		return 0;
	if (!n)
		goto ilseq;
	if (!wc)
		wc = (void *)&wc;

	if (*s < 0x80)
		return !!(*wc = *s);
	if ((*s - __SA) > (__SB - __SA))
		goto ilseq;
	c = bittab[*s++ - __SA];

	/* Avoid excessive checks against n: If shifting the state n-1
     * times does not clear the high bit, then the value of n is
     * insufficient to read a character */
	if (n < 4 && ((c << (6 * n - 6)) & (1U << 31)))
		goto ilseq;

	if (__OOB(c, *s))
		goto ilseq;
	c = ((c << 6) | (*s++ - 0x80U));
	if (!(c & (1U << 31)))
	{
		*wc = (wchar_t)c;
		return 2;
	}

	if ((*s - 0x80U) >= 0x40)
		goto ilseq;
	c = ((c << 6) | (*s++ - 0x80U));
	if (!(c & (1U << 31)))
	{
		*wc = (wchar_t)c;
		return 3;
	}

	if ((*s - 0x80U) >= 0x40)
		goto ilseq;
	*wc = (wchar_t)((c << 6) | (*s++ - 0x80U));
	return 4;

ilseq:
	__set_errno(EILSEQ);
	return -1;
#else
	int retval = 0;
	mbstate_t *ps = &__global_clib2->wide_status->_mbtowc_state;

	retval = _mbtowc(wc, src, n, ps);

	if (retval < 0)
	{
		ps->__count = 0;
		return -1;
	}
	return retval;
#endif
}
