/*
 * $Id: socket_getnameinfo.c,v 1.0 2021-01-13 11:31:35 apalmate Exp $
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

int getnameinfo(const struct sockaddr *sa, socklen_t salen, char *node, socklen_t nodelen, char *service, socklen_t servicelen, int flags)
{
  /* FIXME: Support other flags. */
  if (flags & ~(NI_NUMERICHOST | NI_NUMERICSERV))
    return EAI_BADFLAGS;

  if (sa == NULL || salen < sizeof(sa->sa_family))
    return EAI_FAMILY;

  switch (sa->sa_family)
  {
  case AF_INET:
    if (salen < sizeof(struct sockaddr_in))
      return EAI_FAMILY;
    break;
  case AF_INET6:
    if (salen < sizeof(struct sockaddr_in6))
      return EAI_FAMILY;
    break;
  default:
    return EAI_FAMILY;
  }
  if (node && nodelen > 0 && flags & NI_NUMERICHOST)
  {
    switch (sa->sa_family)
    {
    case AF_INET:
      if (!inet_ntop(AF_INET,
                     &(((const struct sockaddr_in *)sa)->sin_addr),
                     node, nodelen))
        return EAI_SYSTEM;
      break;
    case AF_INET6:
      if (!inet_ntop(AF_INET6,
                     &(((const struct sockaddr_in6 *)sa)->sin6_addr),
                     node, nodelen))
        return EAI_SYSTEM;
      break;
    default:
      return EAI_FAMILY;
    }
  }

  if (service && servicelen > 0 && flags & NI_NUMERICSERV)
    switch (sa->sa_family)
    {
    case AF_INET:
    case AF_INET6:
    {
      unsigned short int port = ntohs(((const struct sockaddr_in *)sa)->sin_port);
      if (servicelen <= (socklen_t)snprintf(service, servicelen, "%u", port))
        return EAI_OVERFLOW;
    }
    break;
    }

  return 0;
}

#endif
