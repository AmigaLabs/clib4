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
wcrtomb(char *s, wchar_t wchar, mbstate_t *state)
{
#ifdef LIBWCHAR
    (void) state;
    if (!s)
        return 1;
    if ((unsigned)wchar < 0x80U)
    {
        *s = (char)wchar;
        return 1;
    }
    else if ((unsigned)wchar < 0x800U)
    {
        *s++ = (char)(0xc0 | (wchar >> 6));
        *s   = (char)(0x80 | (wchar & 0x3f));
        return 2;
    }
    else if (((unsigned)wchar < 0xd800U) || (((unsigned)wchar - 0xe000U) < 0x2000U))
    {
        *s++ = (char)(0xe0 |  (wchar >> 12));
        *s++ = (char)(0x80 | ((wchar >> 6) & 0x3f));
        *s   = (char)(0x80 |  (wchar & 0x3f));
        return 3;
    }
    else if (((unsigned)wchar - 0x10000U) < 0x100000U)
    {
        *s++ = (char)(0xf0 |  (wchar >> 18));
        *s++ = (char)(0x80 | ((wchar >> 12) & 0x3f));
        *s++ = (char)(0x80 | ((wchar >> 6) & 0x3f));
        *s   = (char)(0x80 |  (wchar & 0x3f));
        return 4;
    }
    __set_errno(EILSEQ);
    return (size_t)-1;
#else
	if (strlen(__lc_ctype) <= 1)
	{ /* fall-through */
	}
	else if (!strcmp(__lc_ctype, "C-UTF-8"))
	{
		if (s == NULL)
			return 0; /* UTF-8 encoding is not state-dependent */

		if (wchar <= 0x7f)
		{
			*s = wchar;
			return 1;
		}
		else if (wchar >= 0x80 && wchar <= 0x7ff)
		{
			*s++ = 0xc0 | ((wchar & 0x7c0) >> 6);
			*s = 0x80 | (wchar & 0x3f);
			return 2;
		}
		else if (wchar >= 0x800 && wchar <= 0xffff)
		{
			/* UTF-16 surrogates -- must not occur in normal UCS-4 data */
			if (wchar >= 0xd800 && wchar <= 0xdfff)
				return -1;

			*s++ = 0xe0 | ((wchar & 0xf000) >> 12);
			*s++ = 0x80 | ((wchar & 0xfc0) >> 6);
			*s = 0x80 | (wchar & 0x3f);
			return 3;
		}
		else if (wchar >= 0x10000 && wchar <= 0x1fffff)
		{
			*s++ = 0xf0 | ((wchar & 0x1c0000) >> 18);
			*s++ = 0x80 | ((wchar & 0x3f000) >> 12);
			*s++ = 0x80 | ((wchar & 0xfc0) >> 6);
			*s = 0x80 | (wchar & 0x3f);
			return 4;
		}
		else if (wchar >= 0x200000 && wchar <= 0x3ffffff)
		{
			*s++ = 0xf8 | ((wchar & 0x3000000) >> 24);
			*s++ = 0x80 | ((wchar & 0xfc0000) >> 18);
			*s++ = 0x80 | ((wchar & 0x3f000) >> 12);
			*s++ = 0x80 | ((wchar & 0xfc0) >> 6);
			*s = 0x80 | (wchar & 0x3f);
			return 5;
		}
		else if (wchar >= 0x4000000 && wchar <= 0x7fffffff)
		{
			*s++ = 0xfc | ((wchar & 0x40000000) >> 30);
			*s++ = 0x80 | ((wchar & 0x3f000000) >> 24);
			*s++ = 0x80 | ((wchar & 0xfc0000) >> 18);
			*s++ = 0x80 | ((wchar & 0x3f000) >> 12);
			*s++ = 0x80 | ((wchar & 0xfc0) >> 6);
			*s = 0x80 | (wchar & 0x3f);
			return 6;
		}
		else
			return -1;
	}
	else if (!strcmp(__lc_ctype, "C-SJIS"))
	{
		unsigned char char2 = (unsigned char)wchar;
		unsigned char char1 = (unsigned char)(wchar >> 8);

		if (s == NULL)
			return 0; /* not state-dependent */

		if (char1 != 0x00)
		{
			/* first byte is non-zero..validate multi-byte char */
			if (_issjis1(char1) && _issjis2(char2))
			{
				*s++ = (char)char1;
				*s = (char)char2;
				return 2;
			}
			else
				return -1;
		}
	}
	else if (!strcmp(__lc_ctype, "C-EUCJP"))
	{
		unsigned char char2 = (unsigned char)wchar;
		unsigned char char1 = (unsigned char)(wchar >> 8);

		if (s == NULL)
			return 0; /* not state-dependent */

		if (char1 != 0x00)
		{
			/* first byte is non-zero..validate multi-byte char */
			if (_iseucjp(char1) && _iseucjp(char2))
			{
				*s++ = (char)char1;
				*s = (char)char2;
				return 2;
			}
			else
				return -1;
		}
	}
	else if (!strcmp(__lc_ctype, "C-JIS"))
	{
		int cnt = 0;
		unsigned char char2 = (unsigned char)wchar;
		unsigned char char1 = (unsigned char)(wchar >> 8);

		if (s == NULL)
			return 1; /* state-dependent */

		if (char1 != 0x00)
		{
			/* first byte is non-zero..validate multi-byte char */
			if (_isjis(char1) && _isjis(char2))
			{
				if (state->__state == 0)
				{
					/* must switch from ASCII to JIS state */
					state->__state = 1;
					*s++ = ESC_CHAR;
					*s++ = '$';
					*s++ = 'B';
					cnt = 3;
				}
				*s++ = (char)char1;
				*s = (char)char2;
				return cnt + 2;
			}
			else
				return -1;
		}
		else
		{
			if (state->__state != 0)
			{
				/* must switch from JIS to ASCII state */
				state->__state = 0;
				*s++ = ESC_CHAR;
				*s++ = '(';
				*s++ = 'B';
				cnt = 3;
			}
			*s = (char)char2;
			return cnt + 1;
		}
	}

	if (s == NULL)
		return 0;

	/* otherwise we are dealing with a single byte character */
	*s = (char)wchar;
	return 1;
#endif
}
