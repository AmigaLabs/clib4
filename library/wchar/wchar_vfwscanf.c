/*
 * $Id: wchar_vfwscanf.c,v 1.0 2021-09-28 12:04:27 apalmate Exp $
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

#ifndef _WCTYPE_HEADERS_H
#include <wctype.h>
#endif /* _WCTYPE_HEADERS_H */

#define SIZE_hh -2
#define SIZE_h -1
#define SIZE_def 0
#define SIZE_l 1
#define SIZE_L 2
#define SIZE_ll 3

static void store_int(void *dest, int size, unsigned long long i)
{
    if (!dest)
        return;
    switch (size)
    {
    case SIZE_hh:
        *(char *)dest = i;
        break;
    case SIZE_h:
        *(short *)dest = i;
        break;
    case SIZE_def:
        *(int *)dest = i;
        break;
    case SIZE_l:
        *(long *)dest = i;
        break;
    case SIZE_ll:
        *(long long *)dest = i;
        break;
    }
}

static void *arg_n(va_list ap, unsigned int n)
{
    void *p;
    unsigned int i;
    va_list ap2;
    va_copy(ap2, ap);
    for (i = n; i > 1; i--)
        va_arg(ap2, void *);
    p = va_arg(ap2, void *);
    va_end(ap2);
    return p;
}

static int in_set(const wchar_t *set, int c)
{
    int j;
    const wchar_t *p = set;
    if (*p == '-')
    {
        if (c == '-')
            return 1;
        p++;
    }
    else if (*p == ']')
    {
        if (c == ']')
            return 1;
        p++;
    }
    for (; *p && *p != ']'; p++)
    {
        if (*p == '-' && p[1] && p[1] != ']')
            for (j = p++ [-1]; j < *p; j++)
                if (c == j)
                    return 1;
        if (c == *p)
            return 1;
    }
    return 0;
}

int vfwscanf(FILE *f, const wchar_t *format, va_list ap)
{
    int width;
    int size;
    int alloc;
    const wchar_t *p;
    int c, t;
    char *s = NULL;
    wchar_t *wcs = NULL;
    void *dest = NULL;
    int invert;
    int matches = 0;
    off_t pos = 0, cnt;
    static const char size_pfx[][3] = {"hh", "h", "", "l", "L", "ll"};
    char tmp[3 * sizeof(int) + 10];
    const wchar_t *set;
    size_t i = 0, k = 0;

    flockfile(f);

    fwide(f, 1);

    for (p = format; *p; p++)
    {

        alloc = 0;

        if (iswspace(*p))
        {
            while (iswspace(p[1]))
                p++;
            while (iswspace((c = getwc(f))))
                pos++;
            ungetwc(c, f);
            continue;
        }
        if (*p != '%' || p[1] == '%')
        {
            if (*p == '%')
            {
                p++;
                while (iswspace((c = getwc(f))))
                    pos++;
            }
            else
            {
                c = getwc(f);
            }
            if (c != *p)
            {
                ungetwc(c, f);
                if (c < 0)
                    goto input_fail;
                goto match_fail;
            }
            pos++;
            continue;
        }

        p++;
        if (*p == '*')
        {
            dest = 0;
            p++;
        }
        else if (iswdigit(*p) && p[1] == '$')
        {
            dest = arg_n(ap, *p - '0');
            p += 2;
        }
        else
        {
            dest = va_arg(ap, void *);
        }

        for (width = 0; iswdigit(*p); p++)
        {
            width = 10 * width + *p - '0';
        }

        if (*p == 'm')
        {
            wcs = 0;
            s = 0;
            alloc = !!dest;
            p++;
        }
        else
        {
            alloc = 0;
        }

        size = SIZE_def;
        switch (*p++)
        {
        case 'h':
            if (*p == 'h')
                p++, size = SIZE_hh;
            else
                size = SIZE_h;
            break;
        case 'l':
            if (*p == 'l')
                p++, size = SIZE_ll;
            else
                size = SIZE_l;
            break;
        case 'j':
            size = SIZE_ll;
            break;
        case 'z':
        case 't':
            size = SIZE_l;
            break;
        case 'L':
            size = SIZE_L;
            break;
        case 'd':
        case 'i':
        case 'o':
        case 'u':
        case 'x':
        case 'a':
        case 'e':
        case 'f':
        case 'g':
        case 'A':
        case 'E':
        case 'F':
        case 'G':
        case 'X':
        case 's':
        case 'c':
        case '[':
        case 'S':
        case 'C':
        case 'p':
        case 'n':
            p--;
            break;
        default:
            goto fmt_fail;
        }

        t = *p;

        /* Transform S,C -> ls,lc */
        if ((t & 0x2f) == 3)
        {
            size = SIZE_l;
            t |= 32;
        }

        if (t != 'n')
        {
            if (t != '[' && (t | 32) != 'c')
                while (iswspace((c = getwc(f))))
                    pos++;
            else
                c = getwc(f);
            if (c < 0)
                goto input_fail;
            ungetwc(c, f);
        }

        switch (t)
        {
        case 'n':
            store_int(dest, size, pos);
            /* do not increment match count, etc! */
            continue;

        case 's':
        case 'c':
        case '[':
            if (t == 'c')
            {
                if (width < 1)
                    width = 1;
                invert = 1;
                set = L"";
            }
            else if (t == 's')
            {
                invert = 1;
                static const wchar_t spaces[] = {
                    ' ', '\t', '\n', '\r', 11, 12, 0x0085,
                    0x2000, 0x2001, 0x2002, 0x2003, 0x2004, 0x2005,
                    0x2006, 0x2008, 0x2009, 0x200a,
                    0x2028, 0x2029, 0x205f, 0x3000, 0};
                set = spaces;
            }
            else
            {
                if (*++p == '^')
                    p++, invert = 1;
                else
                    invert = 0;
                set = p;
                if (*p == ']')
                    p++;
                while (*p != ']')
                {
                    if (!*p)
                        goto fmt_fail;
                    p++;
                }
            }

            s = (size == SIZE_def) ? dest : 0;
            wcs = (size == SIZE_l) ? dest : 0;

            int gotmatch = 0;

            if (width < 1)
                width = -1;

            i = 0;
            if (alloc)
            {
                k = t == 'c' ? width + 1U : 31;
                if (size == SIZE_l)
                {
                    wcs = malloc(k * sizeof(wchar_t));
                    if (!wcs)
                        goto alloc_fail;
                }
                else
                {
                    s = malloc(k);
                    if (!s)
                        goto alloc_fail;
                }
            }
            while (width)
            {
                if ((c = getwc(f)) < 0)
                    break;
                if (in_set(set, c) == invert)
                    break;
                if (wcs)
                {
                    wcs[i++] = c;
                    if (alloc && i == k)
                    {
                        k += k + 1;
                        wchar_t *tmp1 = realloc(wcs, k * sizeof(wchar_t));
                        if (!tmp1)
                            goto alloc_fail;
                        wcs = tmp1;
                    }
                }
                else if (size != SIZE_l)
                {
                    int l = wctomb(s ? s + i : tmp, c);
                    if (l < 0)
                        goto input_fail;
                    i += l;
                    if (alloc && i > k - 4)
                    {
                        k += k + 1;
                        char *tmp1 = realloc(s, k);
                        if (!tmp1)
                            goto alloc_fail;
                        s = tmp1;
                    }
                }
                pos++;
                width -= (width > 0);
                gotmatch = 1;
            }
            if (width)
            {
                ungetwc(c, f);
                if (t == 'c' || !gotmatch)
                    goto match_fail;
            }

            if (alloc)
            {
                if (size == SIZE_l)
                    *(wchar_t **)dest = wcs;
                else
                    *(char **)dest = s;
            }
            if (t != 'c')
            {
                if (wcs)
                    wcs[i] = 0;
                if (s)
                    s[i] = 0;
            }
            break;

        case 'd':
        case 'i':
        case 'o':
        case 'u':
        case 'x':
        case 'a':
        case 'e':
        case 'f':
        case 'g':
        case 'A':
        case 'E':
        case 'F':
        case 'G':
        case 'X':
        case 'p':
            if (width < 1)
                width = 0;
            snprintf(tmp, sizeof tmp, "%.*s%.0d%s%c%%lln",
                     1 + !dest, "%*", width, size_pfx[size + 2], t);
            cnt = 0;
            if (fscanf(f, tmp, dest ? dest : &cnt, &cnt) == -1)
                goto input_fail;
            else if (!cnt)
                goto match_fail;
            pos += cnt;
            break;
        default:
            goto fmt_fail;
        }

        if (dest)
            matches++;
    }
    if (0)
    {
    fmt_fail:
    alloc_fail:
    input_fail:
        if (!matches)
            matches--;
    match_fail:
        if (alloc)
        {
            free(s);
            free(wcs);
        }
    }
    funlockfile(f);
    return matches;
}