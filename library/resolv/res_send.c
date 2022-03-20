/* $Id: socket_res_send.c,v 1.0 2022-03-14 10:44:15 clib2devs Exp $

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

#include <pthread.h>
#include <poll.h>
#include "lookup.h"

static void cleanup(void *p) {
    close((int) p);
}

static unsigned long mtime() {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (unsigned long) ts.tv_sec * 1000
           + ts.tv_nsec / 1000000;
}

int
__res_msend_rc(int nqueries, const unsigned char *const *queries,
               const int *qlens, unsigned char *const *answers, int *alens, int asize,
               const struct resolvconf *conf) {

    int fd;
    int timeout, attempts, retry_interval, servfail_retry = 0;
    struct sockaddr_in sa = {0}, ns[MAXNS] = {{0}};
    socklen_t sl = sizeof sa;
    int nns = 0;
    int family = AF_INET;
    int rlen;
    int next;
    int i, j = 0;
    int cs;
    struct pollfd pfd;
    unsigned long t0, t1, t2;

    pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &cs);

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
            return -1;
        }
    }

    /* Get local address and open/bind a socket */
    sa.sin_family = family;
    fd = socket(family, SOCK_DGRAM, 0);
    if (fd < 0 || bind(fd, (void *) &sa, sl) < 0) {
        if (fd >= 0) close(fd);
        pthread_setcancelstate(cs, 0);
        return -1;
    }

    /* Past this point, there are no errors. Each individual query will
     * yield either no reply (indicated by zero length) or an answer
     * packet which is up to the caller to interpret. */

    pthread_cleanup_push(cleanup, (void *) (intptr_t) fd);
    pthread_setcancelstate(cs, 0);

    memset(alens, 0, sizeof *alens * nqueries);

    pfd.fd = fd;
    pfd.events = POLLIN;
    retry_interval = timeout / attempts;
    next = 0;
    t0 = t2 = mtime();
    t1 = t2 - retry_interval;

    for (; t2 - t0 < timeout; t2 = mtime()) {
        if (t2 - t1 >= retry_interval) {
            /* Query all configured namservers in parallel */
            for (i = 0; i < nqueries; i++) {
                if (!alens[i]) {
                    for (j = 0; j < nns; j++) {
                        sendto(fd, queries[i], qlens[i], MSG_NOSIGNAL, (void *) &ns[j], sl);
                    }
                }
            }
            t1 = t2;
            servfail_retry = 2 * nqueries;
        }

        /* Wait for a response, or until time to retry */
        if (poll(&pfd, 1, t1 + retry_interval - t2) <= 0) continue;

        while ((rlen = recvfrom(fd, answers[next], asize, 0, (void *) &sa, (socklen_t[1]) {sl})) >= 0) {
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
            switch (answers[next][3] & 15) {
                case 0:
                case 3:
                    break;
                case 2:
                    if (servfail_retry && servfail_retry--)
                        sendto(fd, queries[i], qlens[i], MSG_NOSIGNAL, (void *) &ns[j], sl);
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

            if (next == nqueries) goto out;
        }
    }

out:
    pthread_cleanup_pop(1);

    return 0;
}

static int
__res_msend(int nqueries, const unsigned char *const *queries, const int *qlens, unsigned char *const *answers,
            int *alens, int asize) {
    struct resolvconf conf;
    if (__get_resolv_conf(&conf, 0, 0) < 0)
        return -1;
    return __res_msend_rc(nqueries, queries, qlens, answers, alens, asize, &conf);
}

int
res_send(const unsigned char *msg, int msglen, unsigned char *answer, int anslen) {
    int r = __res_msend(1, &msg, &msglen, &answer, &anslen, anslen);
    return r < 0 || !anslen ? -1 : anslen;
}