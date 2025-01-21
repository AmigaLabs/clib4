/* $Id: socket_res_query.c,v 1.0 2022-03-14 10:44:15 clib4devs Exp $

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

#define _DEFAULT_SOURCE

#ifndef _SOCKET_HEADERS_H
#include "socket/socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

int
res_query(const char *name, int klass, int type, unsigned char *dest, int len) {
    unsigned char q[280];
    int ql = res_mkquery(0, name, klass, type, 0, 0, 0, q, sizeof q);
    if (ql < 0) return ql;
    int r = res_send(q, ql, dest, len);
    if (r < 12) {
        __set_h_errno(TRY_AGAIN);
        return -1;
    }
    if ((dest[3] & 15) == 3) {
        __set_h_errno(HOST_NOT_FOUND);
        return -1;
    }
    if ((dest[3] & 15) == 0 && !dest[6] && !dest[7]) {
        __set_h_errno(NO_DATA);
        return -1;
    }
    return r;
}