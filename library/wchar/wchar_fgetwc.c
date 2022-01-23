/*
 * $Id: wchar_fgetwc.c,v 1.3 2006-01-08 12:04:27 obarthel Exp $
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

#ifndef _STDIO_HEADERS_H
#include "stdio_headers.h"
#endif /* _STDIO_HEADERS_H */

#ifndef _WCHAR_HEADERS_H
#include "wchar_headers.h"
#endif /* _WCHAR_HEADERS_H */

static wint_t __fgetwc_unlocked_internal(FILE *f)
{
	wchar_t wc;
	int c;
	size_t l = 0;
    unsigned char b;
    int first = 1;

    do {
        b = c = __getc(f);
        if (c < 0) {
            if (!first) {
                f->_flags2 |= __SERR;
                __set_errno(EILSEQ);
            }
            return WEOF;
        }
        l = mbrtowc(&wc, (void *)&b, 1, &f->_mbstate);
        if (l == -1) {
            if (!first) {
                f->_flags2 |= __SERR;
                ungetc(b, f);
            }
            return WEOF;
        }
        first = 0;
    } while (l == -2);

	return wc;
}

static wint_t __fgetwc_unlocked(FILE *f)
{
	if ((f->_flags2 & __SWID) <= 0) 
		fwide(f, 1);
	wchar_t wc = __fgetwc_unlocked_internal(f);
	return wc;
}

wint_t fgetwc(FILE *f)
{
	wint_t c;
	flockfile(f);
	c = __fgetwc_unlocked(f);
	funlockfile(f);
	return c;
}
