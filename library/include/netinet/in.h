#ifndef _NETINET_IN_H
#define _NETINET_IN_H

#include <features.h>
#include <sys/types.h>
#include <sys/socket.h>

__BEGIN_DECLS

#ifdef __GNUC__
 #ifdef __PPC__
  #pragma pack(2)
 #endif
#elif defined(__VBCC__)
 #pragma amiga-align
#endif

/****************************************************************************/

/*
 * Constants and structures defined by the internet system,
 * Per RFC 790, September 1981, and numerous additions.
 */

/*
 * Data types.
 */
typedef unsigned long	in_addr_t;
typedef unsigned short	in_port_t;

/*
 * Protocols
 */
#define	IPPROTO_IP		0		/* dummy for IP */
#define	IPPROTO_ICMP	1		/* control message protocol */
#define	IPPROTO_IGMP	2		/* group mgmt protocol */
#define	IPPROTO_GGP		3		/* gateway^2 (deprecated) */
#define	IPPROTO_TCP		6		/* tcp */
#define	IPPROTO_EGP		8		/* exterior gateway protocol */
#define	IPPROTO_PUP		12		/* pup */
#define	IPPROTO_UDP		17		/* user datagram protocol */
#define	IPPROTO_IDP		22		/* xns idp */
#define	IPPROTO_TP		29 		/* tp-4 w/ class negotiation */
#define IPPROTO_IPV6    41 		/* IPv6 header */
#define IPPROTO_RSVP    46      /* Reservation Protocol.  */
#define IPPROTO_GRE     47      /* General Routing Encapsulation.  */
#define IPPROTO_ESP     50      /* encapsulating security payload.  */
#define IPPROTO_AH      51      /* authentication header.  */
#define	IPPROTO_EON		80		/* ISO cnlp */
#define IPPROTO_MTP     92      /* Multicast Transport Protocol.  */
#define IPPROTO_BEETPH  94      /* IP option pseudo header for BEET.  */
#define	IPPROTO_ENCAP	98		/* encapsulation header */
#define IPPROTO_PIM     103     /* Protocol Independent Multicast.  */
#define IPPROTO_COMP    108     /* Compression Header Protocol.  */
#define IPPROTO_SCTP    132     /* Stream Control Transmission Protocol.  */
#define IPPROTO_UDPLITE 136     /* UDP-Lite protocol.  */
#define IPPROTO_MPLS    137     /* MPLS in IP.  */
#define	IPPROTO_RAW		255		/* raw IP packet */
#define	IPPROTO_MAX		256

#define IPPROTO_HOPOPTS     0   /* IPv6 Hop-by-Hop options.  */
#define IPPROTO_ROUTING     43  /* IPv6 routing header.  */
#define IPPROTO_FRAGMENT    44  /* IPv6 fragmentation header.  */
#define IPPROTO_ICMPV6      58  /* ICMPv6.  */
#define IPPROTO_NONE        59  /* IPv6 no next header.  */
#define IPPROTO_DSTOPTS     60  /* IPv6 destination options.  */
#define IPPROTO_MH          135 /* IPv6 mobility header.  */

/* Standard well-known ports.  */
enum
{
    IPPORT_ECHO = 7,            /* Echo service.  */
    IPPORT_DISCARD = 9,         /* Discard transmissions service.  */
    IPPORT_SYSTAT = 11,         /* System status service.  */
    IPPORT_DAYTIME = 13,        /* Time of day service.  */
    IPPORT_NETSTAT = 15,        /* Network status service.  */
    IPPORT_FTP = 21,            /* File Transfer Protocol.  */
    IPPORT_TELNET = 23,         /* Telnet protocol.  */
    IPPORT_SMTP = 25,           /* Simple Mail Transfer Protocol.  */
    IPPORT_TIMESERVER = 37,     /* Timeserver service.  */
    IPPORT_NAMESERVER = 42,     /* Domain Name Service.  */
    IPPORT_WHOIS = 43,          /* Internet Whois service.  */
    IPPORT_MTP = 57,

    IPPORT_TFTP = 69,           /* Trivial File Transfer Protocol.  */
    IPPORT_RJE = 77,
    IPPORT_FINGER = 79,         /* Finger service.  */
    IPPORT_TTYLINK = 87,
    IPPORT_SUPDUP = 95,         /* SUPDUP protocol.  */


    IPPORT_EXECSERVER = 512,    /* execd service.  */
    IPPORT_LOGINSERVER = 513,   /* rlogind service.  */
    IPPORT_CMDSERVER = 514,
    IPPORT_EFSSERVER = 520,

    /* UDP ports.  */
    IPPORT_BIFFUDP = 512,
    IPPORT_WHOSERVER = 513,
    IPPORT_ROUTESERVER = 520,

    /* Ports less than this value are reserved for privileged processes.  */
    IPPORT_RESERVED = 1024,

    /* Ports greater this value are reserved for (non-privileged) servers.  */
    IPPORT_USERRESERVED = 5000
};

#define MAX_IPv4_STR_LEN	16
#define MAX_IPv6_STR_LEN	64

/*
 * Internet address (a structure for historical reasons)
 */
struct in_addr
{
    in_addr_t s_addr;
};

struct in6_addr
{
    union
    {
        uint8_t __u6_addr8[16];
        uint16_t __u6_addr16[8];
        uint32_t __u6_addr32[4];
    } __in6_u;
#define s6_addr                 __in6_u.__u6_addr8
#ifdef __USE_MISC
    # define s6_addr16              __in6_u.__u6_addr16
# define s6_addr32              __in6_u.__u6_addr32
#endif
};

/*
 * Definitions of bits in internet address integers.
 * On subnets, the decomposition of addresses to host and net parts
 * is done according to subnet mask, not the masks here.
 */
#define	IN_CLASSA(i)			(((unsigned long)(i) & 0x80000000) == 0)
#define	IN_CLASSA_NET			0xff000000
#define	IN_CLASSA_NSHIFT		24
#define	IN_CLASSA_HOST			0x00ffffff
#define	IN_CLASSA_MAX			128

#define	IN_CLASSB(i)			(((unsigned long)(i) & 0xc0000000) == 0x80000000)
#define	IN_CLASSB_NET			0xffff0000
#define	IN_CLASSB_NSHIFT		16
#define	IN_CLASSB_HOST			0x0000ffff
#define	IN_CLASSB_MAX			65536

#define	IN_CLASSC(i)			(((unsigned long)(i) & 0xe0000000) == 0xc0000000)
#define	IN_CLASSC_NET			0xffffff00
#define	IN_CLASSC_NSHIFT		8
#define	IN_CLASSC_HOST			0x000000ff

#define	IN_CLASSD(i)			(((unsigned long)(i) & 0xf0000000) == 0xe0000000)
#define	IN_CLASSD_NET			0xf0000000	/* These ones aren't really */
#define	IN_CLASSD_NSHIFT		28		/* net and host fields, but */
#define	IN_CLASSD_HOST			0x0fffffff	/* routing needn't know.    */
#define	IN_MULTICAST(i)			IN_CLASSD(i)

#define	IN_EXPERIMENTAL(i)		(((unsigned long)(i) & 0xf0000000) == 0xf0000000)
#define	IN_BADCLASS(i)			(((unsigned long)(i) & 0xf0000000) == 0xf0000000)

#define	INADDR_ANY				0x00000000UL
#define	INADDR_BROADCAST		0xffffffffUL	/* must be masked */
#define	INADDR_NONE				0xffffffff		/* -1 return */

/* Defines for Multicast INADDR.  */
#define INADDR_UNSPEC_GROUP     ((in_addr_t) 0xe0000000) /* 224.0.0.0 */
#define INADDR_ALLHOSTS_GROUP   ((in_addr_t) 0xe0000001) /* 224.0.0.1 */
#define INADDR_ALLRTRS_GROUP    ((in_addr_t) 0xe0000002) /* 224.0.0.2 */
#define INADDR_ALLSNOOPERS_GROUP ((in_addr_t) 0xe000006a) /* 224.0.0.106 */
#define INADDR_MAX_LOCAL_GROUP  ((in_addr_t) 0xe00000ff) /* 224.0.0.255 */

#define INADDR_LOOPBACK 		0x7f000001UL	/* 127.0.0.1 */

#define	IN_LOOPBACKNET			127				/* official! */


extern const struct in6_addr in6addr_any;        /* :: */
extern const struct in6_addr in6addr_loopback;   /* ::1 */
#define IN6ADDR_ANY_INIT { { { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 } } }
#define IN6ADDR_LOOPBACK_INIT { { { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1 } } }

#define INET_ADDRSTRLEN 	16

/*
 * Socket address, internet style.
 */
struct sockaddr_in
{
	unsigned char	sin_len;
	unsigned char	sin_family;
	unsigned short	sin_port;
	struct in_addr	sin_addr;
	unsigned char	sin_zero[8];
};

struct sockaddr_in6 {
	unsigned char   sin6_family;   /* AF_INET6 */
	unsigned short  sin6_port;     /* port number */
	unsigned long   sin6_flowinfo; /* IPv6 flow information */
	struct in6_addr sin6_addr;     /* IPv6 address */
	unsigned long   sin6_scope_id; /* Scope ID (new in 2.4) */
};

/*
 * Structure used to describe IP options.
 * Used to store options internally, to pass them to a process,
 * or to restore options retrieved earlier.
 * The ip_dst is used for the first-hop gateway when using a source route
 * (this gets put into the header proper).
 */
struct ip_opts
{
	struct in_addr	ip_dst;			/* first hop, 0 w/o src rt */
	unsigned char	ip_options[40];	/* actually variable in size */
};

/*
 * Options for use with [gs]etsockopt at the IP level.
 * First word of comment is data type; bool is stored in int.
 */
#define	IP_OPTIONS			1    /* buf/ip_opts; set/get IP options */
#define	IP_HDRINCL			2    /* int; header is included with data */
#define	IP_TOS				3    /* int; IP type of service and preced. */
#define	IP_TTL				4    /* int; IP time to live */
#define	IP_RECVOPTS			5    /* bool; receive all IP opts w/dgram */
#define	IP_RECVRETOPTS		6    /* bool; receive IP opts for response */
#define	IP_RECVDSTADDR		7    /* bool; receive IP dst addr w/dgram */
#define	IP_RETOPTS			8    /* ip_opts; set/get IP options */
#define	IP_MULTICAST_IF		9    /* unsigned char; set/get IP multicast i/f  */
#define	IP_MULTICAST_TTL	10   /* unsigned char; set/get IP multicast ttl */
#define	IP_MULTICAST_LOOP	11   /* unsigned char; set/get IP multicast loopback */
#define	IP_ADD_MEMBERSHIP	12   /* ip_mreq; add an IP group membership */
#define	IP_DROP_MEMBERSHIP	13   /* ip_mreq; drop an IP group membership */

#define IP_ROUTER_ALERT    5        /* bool */
#define IP_PKTINFO         8        /* bool */
#define IP_PKTOPTIONS      9
#define IP_PMTUDISC        10        /* obsolete name? */
#define IP_MTU_DISCOVER    10        /* int; see below */
#define IP_RECVERR         11        /* bool */
#define IP_RECVTTL         12        /* bool */
#define IP_RECVTOS         13        /* bool */

/* IP_MTU_DISCOVER arguments.  */
#define IP_PMTUDISC_DONT   0        /* Never send DF frames.  */
#define IP_PMTUDISC_WANT   1        /* Use per route hints.  */
#define IP_PMTUDISC_DO     2        /* Always DF.  */
#define IP_PMTUDISC_PROBE  3        /* Ignore dst pmtu.  */

/*
 * Defaults and limits for options
 */
#define	IP_DEFAULT_MULTICAST_TTL	1	/* normally limit m'casts to 1 hop  */
#define	IP_DEFAULT_MULTICAST_LOOP	1	/* normally hear sends if a member  */
#define	IP_MAX_MEMBERSHIPS			20	/* per socket; must fit in one mbuf */

#ifdef __USE_MISC
/*
 * Argument structure for IP_ADD_MEMBERSHIP and IP_DROP_MEMBERSHIP.
 */
struct ip_mreq
{
	struct in_addr imr_multiaddr;	/* IP multicast address of group */
	struct in_addr imr_interface;	/* local IP address of interface */
};

struct ip_mreq_source
{
    /* IP multicast address of group.  */
    struct in_addr imr_multiaddr;

    /* IP address of interface.  */
    struct in_addr imr_interface;

    /* IP address of source.  */
    struct in_addr imr_sourceaddr;
};

struct ipv6_mreq
{
    /* IPv6 multicast address of group */
    struct in6_addr ipv6mr_multiaddr;

    /* local interface */
    unsigned int ipv6mr_interface;
};

/* Multicast group request.  */
struct group_req
{
    /* Interface index.  */
    uint32_t gr_interface;

    /* Group address.  */
    struct sockaddr_storage gr_group;
};

struct group_source_req
{
    /* Interface index.  */
    uint32_t gsr_interface;

    /* Group address.  */
    struct sockaddr_storage gsr_group;

    /* Source address.  */
    struct sockaddr_storage gsr_source;
};

/* Full-state filter operations.  */
struct ip_msfilter
{
    /* IP multicast address of group.  */
    struct in_addr imsf_multiaddr;

    /* Local IP address of interface.  */
    struct in_addr imsf_interface;

    /* Filter mode.  */
    uint32_t imsf_fmode;

    /* Number of source addresses.  */
    uint32_t imsf_numsrc;
    /* Source addresses.  */
    struct in_addr imsf_slist[1];
};

#define IP_MSFILTER_SIZE(numsrc) (sizeof (struct ip_msfilter) \
                                  - sizeof (struct in_addr)                   \
                                  + (numsrc) * sizeof (struct in_addr))

struct group_filter
{
    /* Interface index.  */
    uint32_t gf_interface;

    /* Group address.  */
    struct sockaddr_storage gf_group;

    /* Filter mode.  */
    uint32_t gf_fmode;

    /* Number of source addresses.  */
    uint32_t gf_numsrc;
    /* Source addresses.  */
    struct sockaddr_storage gf_slist[1];
};

#define GROUP_FILTER_SIZE(numsrc) (sizeof (struct group_filter)         \
                                   - sizeof (struct sockaddr_storage)   \
                                   + ((numsrc)                          \
                                   * sizeof (struct sockaddr_storage)))

#endif

/*
 * Definitions for inet sysctl operations.
 *
 * Third level is protocol number.
 * Fourth level is desired variable within that protocol.
 */
#define	IPPROTO_MAXID	(IPPROTO_IDP + 1)	/* don't list to IPPROTO_MAX */

/*
 * Names for IP sysctl objects
 */
#define	IPCTL_FORWARDING	1	/* act as router */
#define	IPCTL_SENDREDIRECTS	2	/* may send redirects when forwarding */
#define	IPCTL_DEFTTL		3	/* default TTL */
#ifdef notyet
#define	IPCTL_DEFMTU		4	/* default MTU */
#endif
#define	IPCTL_MAXID			5

/****************************************************************************/

/*
 * Macros for network/external number representation conversion.
 */
#define	ntohl(x) (x)
#define	ntohs(x) (x)
#define	htonl(x) (x)
#define	htons(x) (x)

#define	NTOHL(x) (x)
#define	NTOHS(x) (x)
#define	HTONL(x) (x)
#define	HTONS(x) (x)

/* Specific IPV6 macros */

#define INET6_ADDRSTRLEN 46

#define IN6_ARE_ADDR_EQUAL(a, b) \
        (((const uint32_t *)(a))[0] == ((const uint32_t *)(b))[0] \
         && ((const uint32_t *)(a))[1] == ((const uint32_t *)(b))[1] \
         && ((const uint32_t *)(a))[2] == ((const uint32_t *)(b))[2] \
         && ((const uint32_t *)(a))[3] == ((const uint32_t *)(b))[3])

#define IN6_IS_ADDR_UNSPECIFIED(addr) \
        (((const uint32_t *)(addr))[0] == 0 \
         && ((const uint32_t *)(addr))[1] == 0 \
         && ((const uint32_t *)(addr))[2] == 0 \
         && ((const uint32_t *)(addr))[3] == 0)

#define IN6_IS_ADDR_LOOPBACK(addr) \
        (((const uint32_t *)(addr))[0] == 0 \
         && ((const uint32_t *)(addr))[1] == 0 \
         && ((const uint32_t *)(addr))[2] == 0 \
         && ((const uint32_t *)(addr))[3] == htonl (1))

#define IN6_IS_ADDR_MULTICAST(addr) (((const uint8_t *) (addr))[0] == 0xff)

#define IN6_IS_ADDR_LINKLOCAL(addr) \
        ((((const uint16_t *)(addr))[0] & htons (0xffc0)) == htons (0xfe80))

#define IN6_IS_ADDR_SITELOCAL(addr) \
        ((((const uint16_t *)(addr))[0] & htons (0xffc0)) == htons (0xfec0))

#define IN6_IS_ADDR_V4MAPPED(addr) \
        (((const uint32_t *)(addr))[0] == 0 \
         && ((const uint32_t *)(addr))[1] == 0 \
         && ((const uint32_t *)(addr))[2] == htonl (0xffff))

#define IN6_IS_ADDR_V4COMPAT(addr) \
        (((const uint32_t *)(addr))[0] == 0 \
         && ((const uint32_t *)(addr))[1] == 0 \
         && ((const uint32_t *)(addr))[2] == 0 \
         && ntohl (((const uint32_t *)(addr))[3]) > 1)

#define IN6_IS_ADDR_MC_NODELOCAL(addr) \
        (IN6_IS_ADDR_MULTICAST(addr) \
         && (((const uint8_t *)(addr))[1] & 0xf) == 0x1)

#define IN6_IS_ADDR_MC_LINKLOCAL(addr) \
        (IN6_IS_ADDR_MULTICAST (addr) \
         && (((const uint8_t *)(addr))[1] & 0xf) == 0x2)

#define IN6_IS_ADDR_MC_SITELOCAL(addr) \
        (IN6_IS_ADDR_MULTICAST(addr) \
         && (((const uint8_t *)(addr))[1] & 0xf) == 0x5)

#define IN6_IS_ADDR_MC_ORGLOCAL(addr) \
        (IN6_IS_ADDR_MULTICAST(addr) \
         && (((const uint8_t *)(addr))[1] & 0xf) == 0x8)

#define IN6_IS_ADDR_MC_GLOBAL(addr) \
        (IN6_IS_ADDR_MULTICAST(addr) \
         && (((const uint8_t *)(addr))[1] & 0xf) == 0xe)

#define IP_PORTRANGE		19 /* int; range to choose for unspec port */
#define IPV6_PORTRANGE		14 /* int; range to choose for unspec port */

/*
 * Argument for IPV6_PORTRANGE:
 * - which range to search when port is unspecified at bind() or connect()
 */
#define	IPV6_PORTRANGE_DEFAULT	0	/* default range */
#define	IPV6_PORTRANGE_HIGH	1	/* "high" - request firewall bypass */
#define	IPV6_PORTRANGE_LOW	2	/* "low" - vouchsafe security */

/*
 * Argument for IP_PORTRANGE:
 * - which range to search when port is unspecified at bind() or connect()
 */
#define	IP_PORTRANGE_DEFAULT	0	/* default range */
#define	IP_PORTRANGE_HIGH	1	/* "high" - request firewall bypass */
#define	IP_PORTRANGE_LOW	2	/* "low" - vouchsafe security */

#define IPV6_UNICAST_HOPS	4  /* int; IP6 hops */
#define IPV6_MULTICAST_IF	9  /* __uint8_t; set/get IP6 multicast i/f  */
#define IPV6_MULTICAST_HOPS	10 /* __uint8_t; set/get IP6 multicast hops */
#define IPV6_MULTICAST_LOOP	11 /* __uint8_t; set/get IP6 mcast loopback */
#define IPV6_JOIN_GROUP		12 /* ip6_mreq; join a group membership */
#define IPV6_LEAVE_GROUP	13 /* ip6_mreq; leave a group membership */
#define IPV6_V6ONLY         26 /* If this flag is set to true (nonzero), then the socket is
                                  restricted to sending and receiving IPv6 packets only */
#ifdef __USE_MISC
/* Bind socket to a privileged IP port.  */
extern int bindresvport(int sd, struct sockaddr_in *sa);

/* The IPv6 version of this function.  */
extern int bindresvport6(int sd, struct sockaddr_in6 *sa);
#endif

/* IPv6 packet information.  */
struct in6_pktinfo
{
    struct in6_addr ipi6_addr;  /* src/dst IPv6 address */
    unsigned int ipi6_ifindex;  /* send/recv interface index */
};

/* IPv6 MTU information.  */
struct ip6_mtuinfo
{
    struct sockaddr_in6 ip6m_addr; /* dst address including zone ID */
    uint32_t ip6m_mtu;             /* path MTU in host byte order */
};


#ifdef __GNUC__
 #ifdef __PPC__
  #pragma pack()
 #endif
#elif defined(__VBCC__)
 #p #pragma default-al
#endif

__END_DECLS

#endif /* _NETINET_IN_H */
