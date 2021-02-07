/*
 * $Id: wchar_wcsrtombs.c,v 1.3 2006-01-08 12:04:27 obarthel Exp $
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
wcsrtombs(char *s, const wchar_t **ws, size_t n, mbstate_t *st)
{
#ifdef LIBWCHAR
	(void)st;
	char buf[2]; // original 4
	size_t N = n, l;
	if (!s)
	{
		const wchar_t *ws2;
		for (n = 0, ws2 = *ws; *ws2; ws2++)
		{
			if (*ws2 >= 0x80u)
			{
				if (!(l = wcrtomb(buf, *ws2, 0)))
				{
					return (size_t)-1;
				}
				n += l;
			}
			else
			{
				n++;
			}
		}
		return n;
	}
	if (n < 2)
	{
		return (size_t)-1;
	}
	while ((n >= 2) && (**ws))
	{
		if (**ws >= 0x80u)
		{
			if (!(l = wcrtomb(s, **ws, 0)))
			{
				return (size_t)-1;
			}
			s += l;
			n -= l;
		}
		else
		{
			*s++ = (char)**ws;
			n--;
		}
		(*ws)++;
	}
	while (n && **ws)
	{
		if (**ws >= 0x80u)
		{
			if (!(l = wcrtomb(buf, **ws, 0)))
			{
				return (size_t)-1;
			}
			if (l > n)
			{
				return (N - n);
			}
			wcrtomb(s, **ws, 0);
			s += l;
			n -= l;
		}
		else
		{
			*s++ = (char)**ws;
			n--;
		}
		(*ws)++;
	}
	if (n)
		*s = 0;
	*ws = 0;
	return (N - n);
#else
	return wcsnrtombs(s, ws, (size_t) -1, n, st);
#endif
}
