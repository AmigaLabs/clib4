/*
 * $Id: socket_socketpair.c,v 1.0 2021-01-24 18:27:15 apalmate Exp $
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

#if defined(SOCKET_SUPPORT)

/****************************************************************************/

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

/****************************************************************************/
static int msnprintf(char *str, size_t str_m, const char *fmt, /*args*/...)
{
    va_list ap;
    int str_l;

    va_start(ap, fmt);
    str_l = vsnprintf(str, str_m, fmt, ap);
    va_end(ap);
    if (str_m)
        str[str_m - 1] = '\0';
    return str_l;
}

int socketpair(int domain, int type, int protocol, int socks[2])
{
    union
    {
        struct sockaddr_in inaddr;
        struct sockaddr addr;
    } a;
    int listener;
    socklen_t addrlen = sizeof(a.inaddr);
    int reuse = 1;
    char data[2][12];
    ssize_t dlen;
    (void)domain;
    (void)type;
    (void)protocol;

    __set_errno(0);

    listener = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (listener == -1)
        return -1;

    memset(&a, 0, sizeof(a));
    a.inaddr.sin_family = AF_INET;
    a.inaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    a.inaddr.sin_port = 0;

    socks[0] = socks[1] = -1;

    if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, (int)sizeof(reuse)) == -1)
        goto error;
    if (bind(listener, &a.addr, sizeof(a.inaddr)) == -1)
        goto error;
    if (getsockname(listener, &a.addr, &addrlen) == -1)
        goto error;
    if (listen(listener, 1) == -1)
        goto error;
    socks[0] = socket(AF_INET, SOCK_STREAM, 0);
    if (socks[0] == -1)
        goto error;
    if (connect(socks[0], &a.addr, sizeof(a.inaddr)) == -1)
        goto error;
    socks[1] = accept(listener, NULL, NULL);
    if (socks[1] == -1)
        goto error;

    /* verify that nothing else connected */
    msnprintf(data[0], sizeof(data[0]), "%p", socks);
    dlen = strlen(data[0]);
    if (write(socks[0], data[0], dlen) != dlen)
        goto error;
    if (read(socks[1], data[1], sizeof(data[1])) != dlen)
        goto error;
    if (memcmp(data[0], data[1], dlen))
        goto error;

    close(listener);
    return 0;

error:
    __set_errno(EFAULT);
    close(listener);
    close(socks[0]);
    close(socks[1]);
    return -1;
}

#endif