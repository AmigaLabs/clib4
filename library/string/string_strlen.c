/*
 * $Id: string_strlen.c,v 1.4 2006-01-08 12:04:27 obarthel Exp $
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

#ifndef _STRING_HEADERS_H
#include "string_headers.h"
#endif /* _STRING_HEADERS_H */

INLINE STATIC size_t
__strlen(const char *str)
{
    uint32 *s32, x, magic1, magic2;
    char *s = (char *)str;

    magic1 = 0xFEFEFEFF;
    magic2 = 0x7F7F7F7F;

    s32 = (uint32 *)((uint32)s & ~3);

    x = (s32 == (uint32 *)s)
        ? *s32
        : *s32 | (0xFFFFFFFF << (3-((uint32)s&3)+1)*8);

    while (!((x + magic1) & ~(x | magic2)))
    {
        x = *++s32;
    }

    s = (char *)s32;
    if (x & 0xFF000000)
    {
        s++;
        if (x & 0x00FF0000)
        {
            s++;
            if (x & 0x0000FF00)
            {
                s++;
            }
        }
    }

    return s-str;
}

size_t
strlen(const char *s)
{
	const char *start = s;
	size_t result = 0;

	assert(s != NULL);

	if (s == NULL)
	{
		__set_errno(EFAULT);
		goto out;
	}

	/* Make sure __global_clib2 has been created */
	if (__global_clib2 != NULL)
	{
		switch (__global_clib2->cpufamily)
		{
			case CPUFAMILY_4XX:
				result = __strlen440(s);
				break;
			default:
				result = __strlen(s);
		}
	}
	else {
		/* Fallback to standard function */
		result = __strlen(s);
	}
out:

	return (result);
}
