/* $Id: socket_dn_expand.c,v 1.0 2022-03-14 10:44:15 apalmate Exp $

   Copyright (C) 2005-2006, 2008-2020 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, see <https://www.gnu.org/licenses/>.  */

/*
 * Copyright (c) 1996-1999 by Internet Software Consortium.
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND INTERNET SOFTWARE CONSORTIUM DISCLAIMS
 * ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL INTERNET SOFTWARE
 * CONSORTIUM BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 */

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

int
dn_expand(unsigned char *base, unsigned char *end, unsigned char *src, char *dest, int space) {
    const unsigned char *p = src;
    char *dend, *dbegin = dest;
    int len = -1, i, j;
    if (p == end || space <= 0) return -1;
    dend = dest + (space > 254 ? 254 : space);
    /* detect reference loop using an iteration counter */
    for (i = 0; i < end - base; i += 2) {
        /* loop invariants: p<end, dest<dend */
        if (*p & 0xc0) {
            if (p + 1 == end) return -1;
            j = ((p[0] & 0x3f) << 8) | p[1];
            if (len < 0) len = p + 2 - src;
            if (j >= end - base) return -1;
            p = base + j;
        } else if (*p) {
            if (dest != dbegin) *dest++ = '.';
            j = *p++;
            if (j >= end - p || j >= dend - dest) return -1;
            while (j--) *dest++ = *p++;
        } else {
            *dest = 0;
            if (len < 0) len = p + 1 - src;
            return len;
        }
    }
    return -1;
}