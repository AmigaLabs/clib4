/* $Id: socket_lookup_serv.c,v 1.0 2022-03-14 10:44:15 clib4devs Exp $

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
#include "socket/socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

#include "lookup.h"

int
__lookup_serv(struct service buf[static MAXSERVS], const char *name, int proto, int socktype, int flags) {
    char line[128];
    int cnt = 0;
    char *p, *z = (char *) "";
    unsigned long port = 0;

    switch (socktype) {
        case SOCK_STREAM:
            switch (proto) {
                case 0:
                    proto = IPPROTO_TCP;
                case IPPROTO_TCP:
                    break;
                default:
                    return EAI_SERVICE;
            }
            break;
        case SOCK_DGRAM:
            switch (proto) {
                case 0:
                    proto = IPPROTO_UDP;
                case IPPROTO_UDP:
                    break;
                default:
                    return EAI_SERVICE;
            }
        case 0:
            break;
        default:
            if (name) return EAI_SERVICE;
            buf[0].port = 0;
            buf[0].proto = proto;
            buf[0].socktype = socktype;
            return 1;
    }

    if (name) {
        if (!*name) return EAI_SERVICE;
        port = strtoul(name, &z, 10);
    }
    if (!*z) {
        if (port > 65535) return EAI_SERVICE;
        if (proto != IPPROTO_UDP) {
            buf[cnt].port = port;
            buf[cnt].socktype = SOCK_STREAM;
            buf[cnt++].proto = IPPROTO_TCP;
        }
        if (proto != IPPROTO_TCP) {
            buf[cnt].port = port;
            buf[cnt].socktype = SOCK_DGRAM;
            buf[cnt++].proto = IPPROTO_UDP;
        }
        return cnt;
    }

    if (flags & AI_NUMERICSERV) return EAI_NONAME;

    size_t l = strlen(name);

    unsigned char _buf[1032];
    FILE *f = fopen(_PATH_SERVICES, "r");
    if (!f)
        switch (errno) {
            case ENOENT:
            case ENOTDIR:
            case EACCES:
                return EAI_SERVICE;
            default:
                return EAI_SYSTEM;
        }

    while (fgets(line, sizeof line, f) && cnt < MAXSERVS) {
        if ((p = strchr(line, '#'))) *p++ = '\n', *p = 0;

        /* Find service name */
        for (p = line; (p = strstr(p, name)); p++) {
            if (p > line && !isspace(p[-1])) continue;
            if (p[l] && !isspace(p[l])) continue;
            break;
        }
        if (!p) continue;

        /* Skip past canonical name at beginning of line */
        for (p = line; *p && !isspace(*p); p++);

        port = strtoul(p, &z, 10);
        if (port > 65535 || z == p) continue;
        if (!strncmp(z, "/udp", 4)) {
            if (proto == IPPROTO_TCP) continue;
            buf[cnt].port = port;
            buf[cnt].socktype = SOCK_DGRAM;
            buf[cnt++].proto = IPPROTO_UDP;
        }
        if (!strncmp(z, "/tcp", 4)) {
            if (proto == IPPROTO_UDP) continue;
            buf[cnt].port = port;
            buf[cnt].socktype = SOCK_STREAM;
            buf[cnt++].proto = IPPROTO_TCP;
        }
    }
    fclose(f);
    return cnt > 0 ? cnt : EAI_SERVICE;
}
