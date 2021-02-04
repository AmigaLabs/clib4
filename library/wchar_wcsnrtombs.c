/*
 * $Id: wchar_wcsnrtombs.c,v 1.0 2021-02-04 11:26:27 apalmate Exp $
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
wcsnrtombs(char *dst, const wchar_t **src, size_t nwc, size_t len, mbstate_t *ps)
{
    char *ptr = dst;
    char buff[10] = {0};
    wchar_t *pwcs;
    size_t n;
    int i;

    if (ps == NULL)
    {
        ps = &__global_clib2->wide_status->_wcsrtombs_state;
    }

    /* If no dst pointer, treat len as maximum possible value. */
    if (dst == NULL)
        len = (size_t)-1;

    n = 0;
    pwcs = (wchar_t *)(*src);

    while (n < len && nwc-- > 0)
    {
        int count = ps->__count;
        wint_t wch = ps->__value.__wch;
        int bytes = wcrtomb(buff, *pwcs, ps);
        if (bytes == -1)
        {
            __set_errno(EILSEQ);
            ps->__count = 0;
            return (size_t)-1;
        }
        if (n + bytes <= len)
        {
            n += bytes;
            if (dst)
            {
                for (i = 0; i < bytes; ++i)
                    *ptr++ = buff[i];
                ++(*src);
            }
            if (*pwcs++ == 0x00)
            {
                if (dst)
                    *src = NULL;
                ps->__count = 0;
                return n - 1;
            }
        }
        else
        {
            /* not enough room, we must back up state to before __WCTOMB call */
            ps->__count = count;
            ps->__value.__wch = wch;
            len = 0;
        }
    }

    return n;
}
