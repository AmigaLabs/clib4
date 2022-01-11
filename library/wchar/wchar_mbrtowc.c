/*
 * $Id: wchar_mbrlen.c,v 1.4 2021-02-03 19:11:16 apalmate Exp $
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

size_t
mbrtowc(wchar_t *restrict pwc, const char *restrict src, size_t n, mbstate_t *restrict ps)
{
#ifdef LIBWCHAR
	static unsigned is = 0U;
	unsigned int c;
	const unsigned char *s = (const void *)src;
	const unsigned int N = (unsigned int)n;

	if (!ps)
	{
		ps = (void *)&is;
	}
	c = *(unsigned *)ps;

	if (!s)
	{
		if (c)
			goto ilseq;
		return ((size_t)0);
	}
	else if (!pwc)
	{
		pwc = (void *)&pwc;
	}
	if (!n)
	{
		return ((size_t)-2);
	}
	if (!c)
	{
		if (*s < 0x80)
		{
			return !!(*pwc = *s);
		}
		if ((*s - __SA) > (__SB - __SA))
		{
			goto ilseq;
		}
		c = bittab[(*s++ - __SA)];
		n--;
	}
	if (n)
	{
		if (__OOB(c, *s))
			goto ilseq;
	loop:
		c = ((c << 6) | (*s++ - 0x80U));
		n--;
		if (!(c & (1U << 31)))
		{
			*(unsigned *)ps = 0U;
			*pwc = (wchar_t)c;
			return (size_t)(N - n);
		}
		if (n)
		{
			if ((*s - 0x80U) >= 0x40)
			{
				goto ilseq;
			}
			goto loop;
		}
	}

	*(unsigned *)ps = c;
	return ((size_t)-2);
ilseq:
	__set_errno(EILSEQ);
	*(unsigned *)ps = 0;
	return ((size_t)-1);
#else
	int retval = 0;

	if (ps == NULL)
	{
		ps = &__global_clib2->wide_status->_mbrtowc_state;
	}

	if (src == NULL) {
        retval = _mbtowc(NULL, "", 1, ps);
    }
	else {
        retval = _mbtowc(pwc, src, n, ps);
    }

	if (retval == -1)
	{
		ps->__count = 0;
		__set_errno(EILSEQ);
		return (size_t)(-1);
	}
	else
		return (size_t)retval;
#endif
}
