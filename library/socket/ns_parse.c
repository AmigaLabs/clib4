/* $Id: socket_ns_parse.c,v 1.0 2022-03-14 10:44:15 clib2devs Exp $

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
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

const struct _ns_flagdata _ns_flagdata[16] = {
                    {0x8000, 15},
                    {0x7800, 11},
                    {0x0400, 10},
                    {0x0200, 9},
                    {0x0100, 8},
                    {0x0080, 7},
                    {0x0040, 6},
                    {0x0020, 5},
                    {0x0010, 4},
                    {0x000f, 0},
                    {0x0000, 0},
                    {0x0000, 0},
                    {0x0000, 0},
                    {0x0000, 0},
                    {0x0000, 0},
                    {0x0000, 0},
            };

unsigned
ns_get16(const unsigned char *cp) {
    return cp[0] << 8 | cp[1];
}

unsigned long
ns_get32(const unsigned char *cp) {
    return (unsigned) cp[0] << 24 | cp[1] << 16 | cp[2] << 8 | cp[3];
}

void
ns_put16(unsigned s, unsigned char *cp) {
    *cp++ = s >> 8;
    *cp++ = s;
}

void
ns_put32(unsigned long l, unsigned char *cp) {
    *cp++ = l >> 24;
    *cp++ = l >> 16;
    *cp++ = l >> 8;
    *cp++ = l;
}

int
ns_initparse(const unsigned char *msg, int msglen, ns_msg *handle) {
    int i, r;

    handle->_msg = msg;
    handle->_eom = msg + msglen;
    if (msglen < (2 + ns_s_max) * NS_INT16SZ) goto bad;
    NS_GET16(handle->_id, msg);
    NS_GET16(handle->_flags, msg);
    for (i = 0; i < ns_s_max; i++) NS_GET16(handle->_counts[i], msg);
    for (i = 0; i < ns_s_max; i++) {
        if (handle->_counts[i]) {
            handle->_sections[i] = msg;
            r = ns_skiprr(msg, handle->_eom, i, handle->_counts[i]);
            if (r < 0) return -1;
            msg += r;
        } else {
            handle->_sections[i] = NULL;
        }
    }
    if (msg != handle->_eom) goto bad;
    handle->_sect = ns_s_max;
    handle->_rrnum = -1;
    handle->_msg_ptr = NULL;
    return 0;
    bad:
    errno = EMSGSIZE;
    return -1;
}

int
ns_skiprr(const unsigned char *ptr, const unsigned char *eom, ns_sect section, int count) {
    const unsigned char *p = ptr;
    int r;

    while (count--) {
        r = dn_skipname(p, eom);
        if (r < 0) goto bad;
        if (r + 2 * NS_INT16SZ > eom - p) goto bad;
        p += r + 2 * NS_INT16SZ;
        if (section != ns_s_qd) {
            if (NS_INT32SZ + NS_INT16SZ > eom - p) goto bad;
            p += NS_INT32SZ;
            NS_GET16(r, p);
            if (r > eom - p) goto bad;
            p += r;
        }
    }
    return p - ptr;
    bad:
    errno = EMSGSIZE;
    return -1;
}

int
ns_parserr(ns_msg *handle, ns_sect section, int rrnum, ns_rr *rr) {
    int r;

    if (section < 0 || section >= ns_s_max) goto bad;
    if (section != handle->_sect) {
        handle->_sect = section;
        handle->_rrnum = 0;
        handle->_msg_ptr = handle->_sections[section];
    }
    if (rrnum == -1) rrnum = handle->_rrnum;
    if (rrnum < 0 || rrnum >= handle->_counts[section]) goto bad;
    if (rrnum < handle->_rrnum) {
        handle->_rrnum = 0;
        handle->_msg_ptr = handle->_sections[section];
    }
    if (rrnum > handle->_rrnum) {
        r = ns_skiprr(handle->_msg_ptr, handle->_eom, section, rrnum - handle->_rrnum);
        if (r < 0) return -1;
        handle->_msg_ptr += r;
        handle->_rrnum = rrnum;
    }
    r = ns_name_uncompress(handle->_msg, handle->_eom, handle->_msg_ptr, rr->name, NS_MAXDNAME);
    if (r < 0) return -1;
    handle->_msg_ptr += r;
    if (2 * NS_INT16SZ > handle->_eom - handle->_msg_ptr) goto size;
    NS_GET16(rr->type, handle->_msg_ptr);
    NS_GET16(rr->rr_class, handle->_msg_ptr);
    if (section != ns_s_qd) {
        if (NS_INT32SZ + NS_INT16SZ > handle->_eom - handle->_msg_ptr) goto size;
        NS_GET32(rr->ttl, handle->_msg_ptr);
        NS_GET16(rr->rdlength, handle->_msg_ptr);
        if (rr->rdlength > handle->_eom - handle->_msg_ptr) goto size;
        rr->rdata = handle->_msg_ptr;
        handle->_msg_ptr += rr->rdlength;
    } else {
        rr->ttl = 0;
        rr->rdlength = 0;
        rr->rdata = NULL;
    }
    handle->_rrnum++;
    if (handle->_rrnum > handle->_counts[section]) {
        handle->_sect = section + 1;
        if (handle->_sect == ns_s_max) {
            handle->_rrnum = -1;
            handle->_msg_ptr = NULL;
        } else {
            handle->_rrnum = 0;
        }
    }
    return 0;
    bad:
    errno = ENODEV;
    return -1;
    size:
    errno = EMSGSIZE;
    return -1;
}

int
ns_name_uncompress(const unsigned char *msg, const unsigned char *eom, const unsigned char *src, char *dst, size_t dstsiz) {
    int r;
    r = dn_expand((unsigned char *)msg, (unsigned char*) eom, (unsigned char *) src, dst, dstsiz);
    if (r < 0) errno = EMSGSIZE;
    return r;
}

