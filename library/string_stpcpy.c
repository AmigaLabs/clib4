/*
 * $Id: string_stpcpy.c,v 1.0 2021-01-21 17:43:27 apalmate Exp $
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

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

#define HIGHS(x) ((x) - ((x)*0 - 1) / 255 * 128)
#define LOWS(x) (((x)*0 - 1) / 255)
#define has_zero(x) ((x)-(LOWS(x) & ~(x)&HIGHS(x)))

/* Copy SRC to DEST, returning the address of the terminating '\0' in DEST.  */
char *
stpcpy(char *dest, const char *src)
{
    size_t *wd;
    const size_t *ws;

    if ((uintptr_t)src % sizeof(size_t) != (uintptr_t)dest % sizeof(size_t))
        goto bytewise;

    for (; (uintptr_t)src % sizeof(size_t); src++, dest++)
        if (!(*dest = *src))
            return dest;

    for (wd = (size_t *)dest, ws = (const size_t *)src; !has_zero(*ws); wd++, ws++, *wd = *ws)
        ;

    dest = (char *)wd;
    src = (const char *)ws;

bytewise:
    for (; (*dest = *src); dest++, src++)
        ;

    return dest;
}