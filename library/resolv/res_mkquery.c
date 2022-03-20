/* $Id: socket_res_mkquery.c,v 1.0 2022-03-14 10:44:15 clib2devs Exp $

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
#include "../socket/socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

#ifndef _TIME_HEADERS_H
#include "time_headers.h"
#endif /* _TIME_HEADERS_H */

int
res_mkquery(int op, const char *dname, int class, int type,
            const unsigned char *data, int datalen,
            const unsigned char *newrr, unsigned char *buf, int buflen) {
    (void) (data);
    (void) (datalen);
    (void) (newrr);

    int id, i, j;
    unsigned char q[280];
    struct timespec ts;
    size_t l = strnlen(dname, 255);
    int n;

    if (l && dname[l - 1] == '.') l--;
    n = 17 + l + !!l;
    if (l > 253 || buflen < n || op > 15u || class > 255u || type > 255u)
        return -1;

    /* Construct query template - ID will be filled later */
    memset(q, 0, n);
    q[2] = op * 8 + 1;
    q[3] = 32; /* AD */
    q[5] = 1;
    memcpy((char *) q + 13, dname, l);
    for (i = 13; q[i]; i = j + 1) {
        for (j = i; q[j] && q[j] != '.'; j++);
        if (j - i - 1u > 62u) return -1;
        q[i - 1] = j - i;
    }
    q[i + 1] = type;
    q[i + 3] = class;

    /* Make a reasonably unpredictable id */
    clock_gettime(CLOCK_REALTIME, &ts);
    id = (ts.tv_nsec + ts.tv_nsec / 65536UL) & 0xffff; // TODO CHECK
    q[0] = id / 256;
    q[1] = id;

    memcpy(buf, q, n);
    return n;
}