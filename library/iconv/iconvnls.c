/*
 * $Id: iconv_iconvnls.c,v 1.0 2021-03-09 12:04:25 clib2devs Exp $
 *
 * Copyright (c) 2003-2004, Artem B. Bityuckiy
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */


#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#include <sys/iconvnls.h>
#include <wchar.h>
#include <iconv.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "local.h"
#include "conv.h"
#include "ucsconv.h"
#include "iconvnls.h"

const char *
_iconv_nls_construct_filename(const char *file, const char *dir, const char *ext)
{
    int len1, len2, len3;
    char *path;
    char *p;
    int dirlen = strlen(dir);

    path = (char *) ICONV_DEFAULT_NLSPATH;

    len1 = strlen(path);
    len2 = strlen(file);
    len3 = strlen(ext);

    if ((p = malloc(len1 + dirlen + len2 + len3 + 3)) == NULL)
        return (const char *)NULL;

    memcpy(p, path, len1);
    if (p[len1 - 1] != '/')
        p[len1++] = '/';
    memcpy(p + len1, dir, dirlen);
    len1 += dirlen;
    p[len1++] = '/';
    memcpy(p + len1, file, len2);
    len1 += len2;
    if (ext != NULL)
    {
        memcpy(p + len1, ext, len3);
        len1 += len3;
    }
    p[len1] = '\0';

    return (const char *)p;
}

int _iconv_nls_get_mb_cur_max(iconv_t cd,
                              int direction)
{
    iconv_conversion_t *ic = (iconv_conversion_t *)cd;

    return ic->handlers->get_mb_cur_max(ic->data, direction);
}

int _iconv_nls_is_stateful(iconv_t cd,
                           int direction)
{
    iconv_conversion_t *ic = (iconv_conversion_t *)cd;

    return ic->handlers->is_stateful(ic->data, direction);
}

size_t
_iconv_nls_conv(iconv_t cd, const char **inbuf, size_t *inbytesleft, char **outbuf, size_t *outbytesleft)
{
    iconv_conversion_t *ic = (iconv_conversion_t *)cd;
    int flags = ICONV_FAIL_BIT;

    if ((void *)cd == NULL || cd == (iconv_t)-1 || ic->data == NULL || (ic->handlers != &_iconv_null_conversion_handlers && ic->handlers != &_iconv_ucs_conversion_handlers))
    {
        __set_errno(EBADF);
        return (size_t)-1;
    }

    if (inbytesleft == NULL || *inbytesleft == 0)
        return (size_t)0;

    if (outbuf == NULL || *outbuf == NULL)
        flags |= ICONV_DONT_SAVE_BIT;

    if (outbytesleft == NULL || *outbytesleft == 0)
    {
        __set_errno(E2BIG);
        return (size_t)-1;
    }

    return ic->handlers->convert(ic->data, (const unsigned char **)inbuf, inbytesleft, (unsigned char **)outbuf, outbytesleft, flags);
}

void _iconv_nls_get_state(iconv_t cd, mbstate_t *ps, int direction)
{
    iconv_conversion_t *ic = (iconv_conversion_t *)cd;

    ic->handlers->get_state(ic->data, ps, direction);

    return;
}

int _iconv_nls_set_state(iconv_t cd, mbstate_t *ps, int direction)
{
    iconv_conversion_t *ic = (iconv_conversion_t *)cd;

    return ic->handlers->set_state(ic->data, ps, direction);
}

static iconv_t
iconv_open1(const char *to, const char *from)
{
    iconv_conversion_t *ic;

    if (to == NULL || from == NULL || *to == '\0' || *from == '\0')
        return (iconv_t)-1;

    ic = (iconv_conversion_t *)malloc(sizeof(iconv_conversion_t));
    if (ic == NULL)
        return (iconv_t)-1;

    /* Select which conversion type to use */
    if (strcmp(from, to) == 0)
    {
        /* Use null conversion */
        ic->handlers = &_iconv_null_conversion_handlers;
        ic->data = ic->handlers->open(to, from);
    }
    else
    {
        /* Use UCS-based conversion */
        ic->handlers = &_iconv_ucs_conversion_handlers;
        ic->data = ic->handlers->open(to, from);
    }

    if (ic->data == NULL)
    {
        free((void *)ic);
        return (iconv_t)-1;
    }

    return (void *)ic;
}

int _iconv_nls_open(const char *encoding, iconv_t *tomb, iconv_t *towc, int flag)
{
    const char *wchar_encoding;

    if (sizeof(wchar_t) > 2 && WCHAR_MAX > 0xFFFF)
        wchar_encoding = "ucs_4_internal";
    else if (sizeof(wchar_t) > 1 && WCHAR_MAX > 0xFF)
        wchar_encoding = "ucs_2_internal";
    else
        wchar_encoding = ""; /* This shuldn't happen */

    if (flag)
    {
        if ((*towc = iconv_open(wchar_encoding, encoding)) == (iconv_t)-1)
            return -1;

        if ((*tomb = iconv_open(encoding, wchar_encoding)) == (iconv_t)-1)
        {
            iconv_close(*towc);
            return -1;
        }
    }
    else
    {
        if ((*towc = iconv_open1(wchar_encoding, encoding)) == (iconv_t)-1)
            return -1;

        if ((*tomb = iconv_open1(encoding, wchar_encoding)) == (iconv_t)-1)
        {
            iconv_close(*towc);
            return -1;
        }
    }

    return 0;
}
