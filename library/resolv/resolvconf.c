/* $Id: resolv_resolvconf.c,v 1.0 2022-03-14 10:44:15 clib4devs Exp $

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

#ifndef _STRING_HEADERS_H
#include "string_headers.h"
#endif /* _STRING_HEADERS_H */

#include <resolv.h>
#include <netinet/in.h>

#include "lookup.h"

int
__get_resolv_conf(struct resolvconf *conf, char *search, size_t search_sz) {
    char line[256] = {0};
    FILE *f;
    int nns = 0;

    conf->ndots = 1;
    conf->timeout = 5;
    conf->attempts = 2;
    if (search) *search = 0;

    f = fopen(_PATH_RESCONF, "r");
    if (!f) {
        switch (errno) {
            case ENOENT:
            case ENOTDIR:
            case EACCES:
                goto no_resolv_conf;
            default:
                return -1;
        }
    }

    while (fgets(line, sizeof line, f)) {
        char *p, *z;

        if (!strncmp(line, "options", 7) && isspace(line[7])) {
            p = strstr(line, "ndots:");
            if (p && isdigit(p[6])) {
                p += 6;
                unsigned long x = strtoul(p, &z, 10);
                if (z != p) conf->ndots = x > 15 ? 15 : x;
            }
            p = strstr(line, "attempts:");
            if (p && isdigit(p[9])) {
                p += 9;
                unsigned long x = strtoul(p, &z, 10);
                if (z != p) conf->attempts = x > 10 ? 10 : x;
            }
            p = strstr(line, "timeout:");
            if (p && (isdigit(p[8]) || p[8] == '.')) {
                p += 8;
                unsigned long x = strtoul(p, &z, 10);
                if (z != p) conf->timeout = x > 60 ? 60 : x;
            }
            continue;
        }
        // Using Devs:Internet/name_resolution we need to check also for =
        if (!strncmp(line, "nameserver", 10) && (isspace(line[10]) || line[10] == '=')) {
            if (nns >= MAXNS)
                continue;
            for (p = line + 11; isspace(*p); p++);
            for (z = p; *z && !isspace(*z); z++);
            *z = 0;
            if (__lookup_ipliteral(conf->ns + nns, p, AF_UNSPEC) > 0)
                nns++;
            continue;
        }

        if (!search) continue;
        if ((strncmp(line, "domain", 6) && strncmp(line, "search", 6))
            || !isspace(line[6]))
            continue;
        for (p = line + 7; isspace(*p); p++);
        size_t l = strlen(p);
        /* This can never happen anyway with chosen buffer sizes. */
        if (l >= search_sz) continue;
        memcpy(search, p, l + 1);
    }

    fclose(f);

no_resolv_conf:
    if (!nns) {
        __lookup_ipliteral(conf->ns, "127.0.0.1", AF_UNSPEC);
        nns = 1;
    }

    conf->nns = nns;

    return 0;
}
