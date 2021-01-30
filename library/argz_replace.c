/*
 * $Id: argz_replace.c,v 1.0 2021-01-21 11:38:20 apalmate Exp $
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
 *
 *****************************************************************************
 *
 * Documentation and source code for this library, and the most recent library
 * build are available from <https://github.com/afxgroup/clib2>.
 *
 *****************************************************************************
 */

#ifndef _STDLIB_HEADERS_H
#include "stdlib_headers.h"
#endif /* _STDLIB_HEADERS_H */

#include <argz.h>

/* Find string str in buffer buf of length buf_len.  Point buf to character after string,
   or set it to NULL if end of buffer is reached.  Return 1 if found, 0 if not. */
static int
_buf_findstr(const char *str, char **buf, size_t *buf_len)
{
    unsigned int i = 0;
    int j = 0;

    for (i = 0; i < *buf_len; i++)
    {
        if (str[0] == (*buf)[i])
        {
            j = i;
            while (str[j - i] && (str[j - i] == (*buf)[j]))
                j++;
            if (str[j - i] == '\0')
            {
                *buf += j;
                *buf_len -= j;
                return 1;
            }
        }
    }

    if (i == *buf_len)
    {
        *buf += *buf_len;
        *buf_len = 0;
    }

    return 0;
}

error_t
argz_replace(char **argz, size_t *argz_len, const char *str, const char *with, unsigned *replace_count)
{
    const int str_len = strlen(str);
    const int with_len = strlen(with);
    const int len_diff = with_len - str_len;

    char *buf_iter = *argz;
    size_t buf_len = *argz_len;
    char *last_iter = NULL;
    char *new_argz = NULL;
    size_t new_argz_len = 0;
    char *new_argz_iter = NULL;

    *replace_count = 0;
    new_argz_len = *argz_len;

    while (buf_len)
    {
        if (_buf_findstr(str, &buf_iter, &buf_len))
        {
            *replace_count += 1;
            new_argz_len += len_diff;
        }
    }

    if (*replace_count)
    {
        new_argz = (char *)malloc(new_argz_len);

        buf_iter = *argz;
        buf_len = *argz_len;
        last_iter = buf_iter;
        new_argz_iter = new_argz;

        while (buf_len)
        {
            if (_buf_findstr(str, &buf_iter, &buf_len))
            {
                /* copy everything up to, but not including str, from old argz to
                 new argz. */
                memcpy(new_argz_iter, last_iter, buf_iter - last_iter - str_len);
                new_argz_iter += (buf_iter - last_iter - str_len);
                /* copy replacement string. */
                memcpy(new_argz_iter, with, with_len);
                new_argz_iter += with_len;
                last_iter = buf_iter;
            }
        }
        /* copy everything after last occurrence of str. */
        memcpy(new_argz_iter, last_iter, *argz + *argz_len - last_iter);

        /* reallocate argz, and copy over the new value. */
        if (!(*argz = (char *)realloc(*argz, new_argz_len)))
            return ENOMEM;

        memcpy(*argz, new_argz, new_argz_len);
        *argz_len = new_argz_len;

        if (*argz_len == 0)
        {
            free(*argz);
            *argz = NULL;
        }
        free(new_argz);
    }

    return 0;
}