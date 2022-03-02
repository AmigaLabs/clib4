/*
 * $Id: wchar_wcsstr.c,v 1.0 2021-02-04 20:31:00 apalmate Exp $
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

wchar_t *
wcsstr(const wchar_t *big, const wchar_t *little)
{
#if 1
    unsigned int i;
    wchar_t *p = (wchar_t *)big;

    if (!p)
        return NULL;

    // Always find the empty string
    if ((!little) || (!little[0]))
        return (wchar_t *)big;

    size_t flen = wcslen(little);

    for (i = 0; i < flen; i++)
    {
        if ((p = wcschr(p, little[i])) == NULL)
            return NULL;

        if (!wmemcmp(p, little, flen))
            return p;

        big = p + 1;
    }
    return NULL;
#else
    const wchar_t *p;
    const wchar_t *q;
    const wchar_t *r;

    if (!*little)
    {
        /* LINTED interface specification */
        return (wchar_t *)big;
    }
    if (wcslen(big) < wcslen(little))
        return NULL;

    p = big;
    q = little;
    while (*p)
    {
        q = little;
        r = p;
        while (*q)
        {
            if (*r != *q)
                break;
            q++;
            r++;
        }
        if (!*q)
        {
            /* LINTED interface specification */
            return (wchar_t *)p;
        }
        p++;
    }
    return NULL;
#endif
}
