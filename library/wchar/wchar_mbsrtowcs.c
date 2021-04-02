/*
 * $Id: wchar_mbsrtowcs.c,v 1.3 2006-01-08 12:04:27 obarthel Exp $
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
mbsrtowcs(wchar_t *restrict ws, const char **restrict src, size_t wn, mbstate_t *restrict st)
{
	const unsigned char *s = (const void *)*src;
	size_t wn0 = wn;
	unsigned c = 0;

	if (st && (c = *(unsigned *)st))
	{
		if (ws)
		{
			*(unsigned *)st = 0;
			goto resume;
		}
		else
		{
			goto resume0;
		}
	}

	if (!ws)
	{
		for (;;)
		{
			if (((*s - 1U) < 0x7f) && (((uintptr_t)s % 2) == 0))
			{
				unsigned short w;
				while (
					(memcpy(&w, s, sizeof(w))) &&
					!((w | (w - 0x01010101U)) & (0x80808080U)))
				{
					s += 2;
					wn -= 2;
				}
			}
			if (*s - 1u < 0x7f)
			{
				s++;
				wn--;
				continue;
			}
			if (*s - __SA > __SB - __SA)
				break;
			c = bittab[*s++ - __SA];
		resume0:
			if (__OOB(c, *s))
			{
				s--;
				break;
			}
			s++;
			if (c & (1U << 25))
			{
				if (*s - 0x80u >= 0x40)
				{
					s -= 2;
					break;
				}
				s++;
				if (c & (1U << 19))
				{
					if (*s - 0x80u >= 0x40)
					{
						s -= 3;
						break;
					}
					s++;
				}
			}
			wn--;
			c = 0;
		}
	}
	else
	{
		for (;;)
		{
			if (!wn)
			{
				return wn0;
			}
			if (((*s - 1U) < 0x7f) && (((uintptr_t)s % 2) == 0))
			{
				unsigned short w;
				while (
					(memcpy(&w, s, sizeof(w))) &&
					(wn >= 2) &&
					!((w | (w - 0x01010101U)) & (0x80808080U)))
				{
					*ws++ = *s++;
					*ws++ = *s++;
					wn -= 2;
				}
			}
			if (*s - 1U < 0x7f)
			{
				*ws++ = *s++;
				wn--;
				continue;
			}
			if (*s - __SA > __SB - __SA)
				break;
			c = bittab[*s++ - __SA];
		resume:
			if (__OOB(c, *s))
			{
				s--;
				break;
			}
			c = ((c << 6) | (*s++ - 0x80U));
			if (c & (1U << 31))
			{
				if (*s - 0x80U >= 0x40)
				{
					s -= 2;
					break;
				}
				c = ((c << 6) | (*s++ - 0x80U));
				if (c & (1U << 31))
				{
					if (*s - 0x80U >= 0x40)
					{
						s -= 3;
						break;
					}
					c = ((c << 6) | (*s++ - 0x80U));
				}
			}
			*ws++ = (wchar_t)c;
			wn--;
			c = 0;
		}
	}

	if (!c && !*s)
	{
		if (ws)
		{
			*ws = 0;
			*src = 0;
		}
		return (wn0 - wn);
	}
	__set_errno(EILSEQ);
	if (ws)
		*src = (const void *)s;
	return (size_t)-1;
}
