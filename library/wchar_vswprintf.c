/*
 * $Id: wchar_vswprintf.c,v 1.3 2006-01-08 12:04:27 obarthel Exp $
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

#include "wchar_wprintf_core.h"

int
vswprintf(wchar_t *restrict s, size_t l, const wchar_t *restrict fmt, va_list ap)
{
    va_list ap2;
    int ret, nl_type[__ARGMAX] = {0};
    union arg nl_arg[__ARGMAX];
    FOut _out[1];

    va_copy(ap2, ap);

    if ((!s) || (!l))
    {
        ret = wprintf_core(0, fmt, &ap2, nl_arg, nl_type);
        va_end(ap2);
        return ret;
    }
    out_init_buffer(_out, s, l);
    ret = wprintf_core(_out, fmt, &ap2, nl_arg, nl_type);
    va_end(ap2);

    if (out_overflow(_out))
        return -1;
    return ret;
}
