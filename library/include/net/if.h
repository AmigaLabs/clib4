#ifndef _NET_IF_H
#define _NET_IF_H

#include <sys/cdefs.h>
#include <sys/types.h>
#include <sys/socket.h>

/* Length of interface name.  */
#define IF_NAMESIZE 16

struct if_nameindex
{
    unsigned int if_index; /* 1, 2, ... */
    char *if_name;         /* null terminated name: "eth0", ... */
};

/* Convert an interface name to an index, and vice versa.  */
extern unsigned int if_nametoindex (const char *ifname);
extern char *if_indextoname (unsigned int ifindex, char *ifname);

/* Return a list of all interfaces and their indices.  */
//extern struct if_nameindex *if_nameindex (void);

/* Free the data returned from if_nameindex.  */
//extern void if_freenameindex (struct if_nameindex *ptr);

#endif