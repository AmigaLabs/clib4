/*
 * $Id: inet.h,v 1.4 2006-01-08 12:06:14 clib2devs Exp $
*/

#ifndef	_ARPA_INET_H
#define	_ARPA_INET_H

#ifndef _NETINET_IN_H
#include <netinet/in.h>
#endif /* _NETINET_IN_H */

#ifndef _SYS_SOCKET_H
#include <sys/socket.h>
#endif

#include <features.h>

__BEGIN_DECLS

#ifndef inet_aton
extern in_addr_t inet_addr(const char *cp);
#endif
#ifndef inet_aton
extern int inet_aton(const char *cp, struct in_addr *pin);
#endif
extern in_addr_t inet_lnaof(struct in_addr in);
extern struct in_addr inet_makeaddr(in_addr_t net, in_addr_t lna);
extern in_addr_t inet_netof(struct in_addr in);
#ifndef inet_aton
extern in_addr_t inet_network(const char *cp);
#endif
extern char *inet_ntoa(struct in_addr in);
#ifndef inet_aton
extern const char *inet_ntop (int af, const void *src, char *dst, socklen_t cnt);
#endif
#ifndef inet_aton
extern int inet_pton (int af, const char *src, void *dst);
#endif

__END_DECLS

#endif /* _ARPA_INET_H */
