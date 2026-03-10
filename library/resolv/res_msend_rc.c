/* $Id: socket_res_msend.c,v 1.0 2022-03-14 10:44:15 clib4devs Exp $

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

#ifndef _TIME_HEADERS_H
#include "time_headers.h"
#endif /* _TIME_HEADERS_H */

#include <sys/socket.h>
#include "lookup.h"

static unsigned long mtime() {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (unsigned long) ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}

int
__res_msend_rc(int nqueries, const unsigned char *const *queries,
               const int *qlens, unsigned char *const *answers, int *alens, int asize,
               const struct resolvconf *conf) {

    int fd;
    int timeout, attempts, retry_interval, servfail_retry = 0;
    struct sockaddr_in sa = {0}, ns[MAXNS] = {{0}};
    socklen_t sl = sizeof sa;
    size_t nns = 0;
    int family = AF_INET;
    int rlen;
    int next;
    int i = 0;
    size_t j = 0;
    int cs;
    unsigned long t0, t1, t2;

    /* Open a per-call bsdsocket.library to get an isolated SocketBase.
     * AmigaOS bsdsocket.library uses per-opener signal masks for WaitSelect.
     * When multiple threads share a single SocketBase, WaitSelect signals
     * interfere with each other causing poll() failures and timeouts.
     * By opening a private instance, each thread gets independent signals. */
    struct Library *localSocketBase = OpenLibrary("bsdsocket.library", 3);
    if (localSocketBase == NULL)
        return -1;
    struct SocketIFace *IS = (struct SocketIFace *)GetInterface(localSocketBase, "main", 1, 0);
    if (IS == NULL) {
        CloseLibrary(localSocketBase);
        return -1;
    }

    timeout = 1000 * conf->timeout;
    attempts = conf->attempts;

    for (nns = 0; nns < conf->nns; nns++) {
        const struct address *iplit = &conf->ns[nns];
        if (iplit->family == AF_INET) {
            memcpy(&ns[nns].sin_addr, iplit->addr, 4);
            ns[nns].sin_port = htons(53);
            ns[nns].sin_family = AF_INET;
        } else {
            __set_errno(EAFNOSUPPORT);
            DropInterface((struct Interface *)IS);
            CloseLibrary(localSocketBase);
            return -1;
        }
    }

    /* Get local address and open/bind a socket using our private SocketBase */
    sa.sin_family = family;

    fd = IS->socket(family, SOCK_DGRAM, 0);
    if (fd < 0 || IS->bind(fd, (struct sockaddr *)&sa, sl) < 0) {
        if (fd >= 0)
            IS->CloseSocket(fd);
        DropInterface((struct Interface *)IS);
        CloseLibrary(localSocketBase);
        return -1;
    }

    /* Past this point, there are no errors. Each individual query will
     * yield either no reply (indicated by zero length) or an answer
     * packet which is up to the caller to interpret. */

    memset(alens, 0, sizeof *alens * nqueries);

    retry_interval = timeout / attempts;
    next = 0;
    t0 = t2 = mtime();
    t1 = t2 - retry_interval;

    for (; t2 - t0 < (unsigned long) timeout; t2 = mtime()) {
        if (t2 - t1 >= (unsigned long) retry_interval) {
            /* Query all configured namservers in parallel */
            for (i = 0; i < nqueries; i++) {
                if (!alens[i]) {
                    for (j = 0; j < nns; j++) {
                        IS->sendto(fd, (APTR)queries[i], qlens[i], 0, (struct sockaddr *)&ns[j], sl);
                    }
                }
            }
            t1 = t2;
            servfail_retry = 2 * nqueries;
        }

        /* Wait for a response using WaitSelect on our private SocketBase */
        fd_set rset;
        struct timeval tv;
        int poll_timeout = (int)(t1 + retry_interval - t2);
        if (poll_timeout < 0) poll_timeout = 0;
        tv.tv_sec = poll_timeout / 1000;
        tv.tv_usec = (poll_timeout % 1000) * 1000;
        FD_ZERO(&rset);
        FD_SET(fd, &rset);

        int poll_ret = IS->WaitSelect(fd + 1, &rset, NULL, NULL, &tv, NULL);
        if (poll_ret <= 0) continue;

        int recv_count = 0;
        while ((rlen = IS->recvfrom(fd, answers[next], asize, MSG_DONTWAIT, (struct sockaddr *)&sa, &sl)) >= 0) {
            recv_count++;
            /* Ignore non-identifiable packets */
            if (rlen < 4) continue;

            /* Find which query this answer goes with, if any */
            for (i = next; i < nqueries && (
                    answers[next][0] != queries[i][0] ||
                    answers[next][1] != queries[i][1]); i++);
            if (i == nqueries) continue;
            if (alens[i]) continue;

            /* Only accept positive or negative responses;
             * retry immediately on server failure, and ignore
             * all other codes such as refusal. */
            int rcode = answers[next][3] & 15;
            switch (rcode) {
                case 0:
                case 3:
                    break;
                case 2:
                    if (servfail_retry && servfail_retry--) {
                        for (j = 0; j < nns; j++)
                            IS->sendto(fd, (APTR)queries[i], qlens[i], MSG_NOSIGNAL, (struct sockaddr *)&ns[j], sl);
                    }
                    /* fall through */
                default:
                    continue;
            }

            /* Store answer in the right slot, or update next
             * available temp slot if it's already in place. */
            alens[i] = rlen;
            if (i == next)
                for (; next < nqueries && alens[next]; next++);
            else
                memcpy(answers[i], answers[next], rlen);

            if (next == nqueries)
                goto out;
        }
    }

    out:
    IS->CloseSocket(fd);
    DropInterface((struct Interface *)IS);
    CloseLibrary(localSocketBase);

    return 0;
}

