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

#include <errno.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <poll.h>

int poll(struct pollfd *fds, nfds_t nfds, int timeout)
{
        fd_set rfds, wfds, efds;
        struct timeval tv, *ptv;
        int max_fd;
        int rc;
        nfds_t i;

        if ((fds == NULL) && (nfds != 0))
        {
                errno = EFAULT;
                return -1;
        }

        FD_ZERO(&rfds);
        FD_ZERO(&wfds);
        FD_ZERO(&efds);

        rc = 0;
        max_fd = 0;

        /* compute fd_sets and find largest descriptor */
        for (i = 0; i < nfds; i++)
        {
                if ((fds[i].fd < 0) || (fds[i].fd >= FD_SETSIZE))
                {
                        fds[i].revents = POLLNVAL;
                        continue;
                }

                if (fds[i].events & (POLLIN | POLLRDNORM))
                {
                        FD_SET(fds[i].fd, &rfds);
                }
                if (fds[i].events & (POLLOUT | POLLWRNORM | POLLWRBAND))
                {
                        FD_SET(fds[i].fd, &wfds);
                }
                if (fds[i].events & (POLLPRI | POLLRDBAND))
                {
                        FD_SET(fds[i].fd, &efds);
                }
                if (fds[i].fd > max_fd &&
                    (fds[i].events & (POLLIN | POLLOUT | POLLPRI |
                                      POLLRDNORM | POLLRDBAND |
                                      POLLWRNORM | POLLWRBAND)))
                {
                        max_fd = fds[i].fd;
                }
        }

        if (timeout < 0)
        {
                ptv = NULL;
        }
        else
        {
                ptv = &tv;
                if (timeout == 0)
                {
                        tv.tv_sec = 0;
                        tv.tv_usec = 0;
                }
                else
                {
                        tv.tv_sec = timeout / 1000;
                        tv.tv_usec = (timeout % 1000) * 1000;
                }
        }

        rc = select(max_fd + 1, &rfds, &wfds, &efds, ptv);
        if (rc < 0)
        {
                return -1;
        }

        for (rc = 0, i = 0; i < nfds; i++)
        {
                if ((fds[i].fd < 0) || (fds[i].fd >= FD_SETSIZE))
                {
                        continue;
                }

                fds[i].revents = 0;

                if (FD_ISSET(fds[i].fd, &rfds))
                {
                        int err = errno;
                        int available = 0;
                        int ret;

                        /* support for POLLHUP */
                        ret = ioctl(fds[i].fd, FIONREAD, &available);
                        if ((ret == -1) || (available == 0))
                        {
                                fds[i].revents |= POLLHUP;
                        }
                        else
                        {
                                fds[i].revents |= fds[i].events & (POLLIN | POLLRDNORM);
                        }

                        errno = err;
                }
                if (FD_ISSET(fds[i].fd, &wfds))
                {
                        fds[i].revents |= fds[i].events & (POLLOUT | POLLWRNORM | POLLWRBAND);
                }
                if (FD_ISSET(fds[i].fd, &efds))
                {
                        fds[i].revents |= fds[i].events & (POLLPRI | POLLRDBAND);
                }
                if (fds[i].revents & ~POLLHUP)
                {
                        rc++;
                }
        }
        return rc;
}
