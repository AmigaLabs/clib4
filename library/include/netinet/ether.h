#ifndef _NETINET_ETHER_H
#define _NETINET_ETHER_H

#include <netinet/if_ether.h>
#include <features.h>

__BEGIN_DECLS

extern char *ether_ntoa (const struct ether_addr *);
extern struct ether_addr *ether_aton (const char *);
extern char *ether_ntoa_r (const struct ether_addr *, char *);
extern struct ether_addr *ether_aton_r (const char *, struct ether_addr *);
extern int ether_line(const char *, struct ether_addr *, char *);
extern int ether_ntohost(char *, const struct ether_addr *);
extern int ether_hostton(const char *, struct ether_addr *);

__END_DECLS

#endif
