/*
 * $Id: poll.c,v 1.0 2021-01-12 11:50:0 apalmate Exp $
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

#ifndef    _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

#include <poll.h>
#include <errno.h>

#ifndef MAX
#define MAX(a, b)    ((a) > (b) ? (a) : (b))
#endif

static int map_poll_spec(struct pollfd *pArray, nfds_t n_fds, fd_set *pReadSet, fd_set *pWriteSet, fd_set *pExceptSet) {
    register nfds_t i;             /* loop control */
    register struct pollfd *pCur;  /* current array element */
    register int max_fd = -1;      /* return value */

    /*
       Map the poll() structures into the file descriptor sets required
       by select().
    */
    for (i = 0, pCur = pArray; i < n_fds; i++, pCur++) {
        /* Skip any bad FDs in the array. */

        if (pCur->fd < 0)
            continue;

        struct fd *fd = __get_file_descriptor(pCur->fd);
        if (pCur->events & POLLIN) {
            /* "Input Ready" notification desired. */
            FD_SET(pCur->fd, pReadSet);
            if (fd != NULL)
                SET_FLAG(fd->fd_Flags, FDF_POLL);
        }

        if (pCur->events & POLLOUT) {
            /* "Output Possible" notification desired. */
            FD_SET(pCur->fd, pWriteSet);
            if (fd != NULL)
                SET_FLAG(fd->fd_Flags, FDF_POLL);
        }

        if (pCur->events & POLLPRI) {
            /* "Exception Occurred" notification desired. (Exceptions include out of band data.) */
            FD_SET(pCur->fd, pExceptSet);
            if (fd != NULL)
                SET_FLAG(fd->fd_Flags, FDF_POLL);
        }

        max_fd = MAX (max_fd, pCur->fd);
    }

    return max_fd;
}

static struct timeval *map_timeout(int poll_timeout, struct timeval *pSelTimeout) {
    struct timeval *pResult;

    /*
       Map the poll() timeout value into a select() timeout.  The possible
       values of the poll() timeout value, and their meanings, are:

       VALUE	MEANING

       -1	wait indefinitely (until signal occurs)
        0	return immediately, don't block
       >0	wait specified number of milliseconds

       select() uses a "struct timeval", which specifies the timeout in
       seconds and microseconds, so the milliseconds value has to be mapped
       accordingly.
    */

    assert(pSelTimeout != (struct timeval *) NULL);

    switch (poll_timeout) {
        case -1:
            /*
               A NULL timeout structure tells select() to wait indefinitely.
            */
            pResult = (struct timeval *) NULL;
            break;
        case 0:
            /*
               "Return immediately" (test) is specified by all zeros in
               a timeval structure.
            */
            pSelTimeout->tv_sec = 0;
            pSelTimeout->tv_usec = 0;
            pResult = pSelTimeout;
            break;
        default:
            /* Wait the specified number of milliseconds. */
            pSelTimeout->tv_sec = poll_timeout / 1000; /* get seconds */
            poll_timeout %= 1000;                /* remove seconds */
            pSelTimeout->tv_usec = poll_timeout * 1000; /* get microseconds */
            pResult = pSelTimeout;
            break;
    }


    return pResult;
}

static void map_select_results(struct pollfd *pArray, unsigned long n_fds, fd_set *pReadSet, fd_set *pWriteSet, fd_set *pExceptSet) {
    register unsigned long i;      /* loop control */
    register struct pollfd *pCur;  /* current array element */

    for (i = 0, pCur = pArray; i < n_fds; i++, pCur++) {
        /* Skip any bad FDs in the array. */
        if (pCur->fd < 0)
            continue;

        /* Exception events take priority over input events. */
        pCur->revents = 0;
        if (FD_ISSET(pCur->fd, pExceptSet))
            pCur->revents |= POLLPRI;

        else if (FD_ISSET(pCur->fd, pReadSet))
            pCur->revents |= POLLIN;

        if (FD_ISSET(pCur->fd, pWriteSet))
            pCur->revents |= POLLOUT;
    }

    return;
}

int
poll(struct pollfd *fds, nfds_t nfds, int timeout) {
    fd_set read_descs;                          /* input file descs */
    fd_set write_descs;                         /* output file descs */
    fd_set except_descs;                        /* exception descs */
    struct timeval stime;                       /* select() timeout value */
    int ready_descriptors;                   /* function result */
    int max_fd;                              /* maximum fd value */
    struct timeval *pTimeout;                   /* actually passed */

    if ((fds == NULL) && (nfds != 0)) {
        __set_errno(EFAULT);
        return -1;
    }

    FD_ZERO(&read_descs);
    FD_ZERO(&write_descs);
    FD_ZERO(&except_descs);

    /* Map the poll() file descriptor list in the select() data structures. */
    max_fd = map_poll_spec(fds, nfds, &read_descs, &write_descs, &except_descs);

    /* Map the poll() timeout value in the select() timeout structure. */
    pTimeout = map_timeout(timeout, &stime);

    /* Make the select() call. */
    ready_descriptors = select(max_fd + 1, &read_descs, &write_descs, &except_descs, pTimeout);

    if (ready_descriptors >= 0) {
        map_select_results(fds, nfds, &read_descs, &write_descs, &except_descs);
    }

    return ready_descriptors;
}
