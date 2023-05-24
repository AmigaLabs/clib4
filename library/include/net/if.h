#ifndef _NET_IF_H
#define _NET_IF_H

#include <features.h>
#include <sys/cdefs.h>
#include <sys/types.h>
#include <sys/socket.h>

/* Length of interface name.  */
#define IF_NAMESIZE 16
#define IFNAMSIZ IF_NAMESIZE

/* Standard interface flags. */
enum
{
    IFF_UP = 0x1,		    /* Interface is up.  */
# define IFF_UP	IFF_UP
    IFF_BROADCAST = 0x2,	/* Broadcast address valid.  */
# define IFF_BROADCAST	    IFF_BROADCAST
    IFF_DEBUG = 0x4,		/* Turn on debugging.  */
# define IFF_DEBUG	        IFF_DEBUG
    IFF_LOOPBACK = 0x8,		/* Is a loopback net.  */
# define IFF_LOOPBACK	    IFF_LOOPBACK
    IFF_POINTOPOINT = 0x10,	/* Interface is point-to-point link.  */
# define IFF_POINTOPOINT    IFF_POINTOPOINT
    IFF_NOTRAILERS = 0x20,	/* Avoid use of trailers.  */
# define IFF_NOTRAILERS	    IFF_NOTRAILERS
    IFF_RUNNING = 0x40,		/* Resources allocated.  */
# define IFF_RUNNING	    IFF_RUNNING
    IFF_NOARP = 0x80,		/* No address resolution protocol.  */
# define IFF_NOARP	        IFF_NOARP
    IFF_PROMISC = 0x100,	/* Receive all packets.  */
# define IFF_PROMISC	    IFF_PROMISC
    /* Not supported */
    IFF_ALLMULTI = 0x200,	/* Receive all multicast packets.  */
# define IFF_ALLMULTI	    IFF_ALLMULTI
    IFF_MASTER = 0x400,		/* Master of a load balancer.  */
# define IFF_MASTER	IFF_MASTER
    IFF_SLAVE = 0x800,		/* Slave of a load balancer.  */
# define IFF_SLAVE	        IFF_SLAVE
    IFF_MULTICAST = 0x1000,	/* Supports multicast.  */
# define IFF_MULTICAST	    IFF_MULTICAST
    IFF_PORTSEL = 0x2000,	/* Can set media type.  */
# define IFF_PORTSEL	    IFF_PORTSEL
    IFF_AUTOMEDIA = 0x4000,	/* Auto media select active.  */
# define IFF_AUTOMEDIA	    IFF_AUTOMEDIA
    IFF_DYNAMIC = 0x8000	/* Dialup device with changing addresses.  */
# define IFF_DYNAMIC	    IFF_DYNAMIC
};

/* The ifaddr structure contains information about one address of an
   interface.  They are maintained by the different address families,
   are allocated and attached when an address is set, and are linked
   together so all addresses for an interface can be located.  */
struct ifaddr
{
    struct sockaddr ifa_addr;	/* Address of interface.  */
    union
    {
        struct sockaddr	ifu_broadaddr;
        struct sockaddr	ifu_dstaddr;
    } ifa_ifu;
    struct iface *ifa_ifp;	/* Back-pointer to interface.  */
    struct ifaddr *ifa_next;	/* Next address for interface.  */
};

/* Device mapping structure. I'd just gone off and designed a
   beautiful scheme using only loadable modules with arguments for
   driver options and along come the PCMCIA people 8)
   Ah well. The get() side of this is good for WDSETUP, and it'll be
   handy for debugging things. The set side is fine for now and being
   very small might be worth keeping for clean configuration.  */

struct ifmap
{
    unsigned long mem_start;
    unsigned long mem_end;
    unsigned short base_addr;
    unsigned char irq;
    unsigned char dma;
    unsigned char port;
    /* 3 bytes spare */
};

struct ifreq
{
    union
    {
        char ifrn_name[IF_NAMESIZE];	/* Interface name, e.g. "en0".  */
    } ifr_ifrn;
    union
    {
        struct sockaddr ifru_addr;
        struct sockaddr ifru_dstaddr;
        struct sockaddr ifru_broadaddr;
        struct sockaddr ifru_netmask;
        struct sockaddr ifru_hwaddr;
        short int ifru_flags;
        int ifru_ivalue;
        int ifru_mtu;
        struct ifmap ifru_map;
        char ifru_slave[IF_NAMESIZE];	/* Just fits the size */
        char ifru_newname[IF_NAMESIZE];
        void *ifru_data;
    } ifr_ifru;
};

#define ifr_name	ifr_ifrn.ifrn_name
#define ifr_hwaddr	ifr_ifru.ifru_hwaddr
#define ifr_addr	ifr_ifru.ifru_addr
#define ifr_dstaddr	ifr_ifru.ifru_dstaddr
#define ifr_broadaddr	ifr_ifru.ifru_broadaddr
#define ifr_netmask	ifr_ifru.ifru_netmask
#define ifr_flags	ifr_ifru.ifru_flags
#define ifr_metric	ifr_ifru.ifru_ivalue
#define ifr_mtu		ifr_ifru.ifru_mtu
#define ifr_map		ifr_ifru.ifru_map
#define ifr_slave	ifr_ifru.ifru_slave
#define ifr_data	ifr_ifru.ifru_data
#define ifr_ifindex	ifr_ifru.ifru_ivalue
#define ifr_bandwidth	ifr_ifru.ifru_ivalue
#define ifr_qlen	ifr_ifru.ifru_ivalue
#define ifr_newname	ifr_ifru.ifru_newname

/* Structure used in SIOCGIFCONF request.  Used to retrieve interface
   configuration for machine (useful for programs which must know all
   networks accessible).  */
struct ifconf
{
    int	ifc_len;			/* Size of buffer.  */
    union
    {
        char *ifcu_buf;
        struct ifreq *ifcu_req;
    } ifc_ifcu;
};

#define ifc_buf		ifc_ifcu.ifcu_buf
#define ifc_req		ifc_ifcu.ifcu_req

struct if_nameindex {
    unsigned int if_index; /* 1, 2, ... */
    char *if_name;         /* null terminated name: "eth0", ... */
};

struct ifaliasreq {
    char   *ifra_name[IF_NAMESIZE];            /* if name, e.g. "en0" */
    struct  sockaddr ifra_addr;
    struct  sockaddr ifra_broadaddr;
    struct  sockaddr ifra_mask;
};


__BEGIN_DECLS

/* Convert an interface name to an index, and vice versa.  */
extern unsigned int if_nametoindex(const char *ifname);
extern char *if_indextoname(unsigned int ifindex, char *ifname);

/* Return a list of all interfaces and their indices.  */
//extern struct if_nameindex *if_nameindex (void);

/* Free the data returned from if_nameindex.  */
//extern void if_freenameindex (struct if_nameindex *ptr);

__END_DECLS

#endif