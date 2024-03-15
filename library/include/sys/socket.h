/*
 * $Id: socket.h,v 1.6 2006-04-04 21:59:13 damato Exp $
*/

#ifndef _SYS_SOCKET_H
#define _SYS_SOCKET_H

#ifndef _STDDEF_H
#include <stddef.h>
#endif /* _STDDEF_H */

#ifndef _SYS_UIO_H_
#include <sys/uio.h>
#endif /* _SYS_UIO_H_ */

#ifndef _SYS_TIME_H
#include <sys/time.h>
#endif /* _SYS_TIME_H */

#include <features.h>

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
 * Definitions related to sockets: types, address families, options.
 */

typedef unsigned long socklen_t;
typedef unsigned char sa_family_t;

/*
 * Types
 */
#define	SOCK_STREAM		1	/* stream socket */
#define	SOCK_DGRAM		2	/* datagram socket */
#define	SOCK_RAW		3	/* raw-protocol interface */
#define	SOCK_RDM		4	/* reliably-delivered message */
#define	SOCK_SEQPACKET	5	/* sequenced packet stream */

#ifndef SOCK_CLOEXEC
#define SOCK_CLOEXEC   02000000
#define SOCK_NONBLOCK  04000
#endif

/*
 * Option flags per-socket.
 */
#define	SO_DEBUG		0x0001	/* turn on debugging info recording */
#define	SO_ACCEPTCONN	0x0002	/* socket has had listen() */
#define	SO_REUSEADDR	0x0004	/* allow local address reuse */
#define	SO_KEEPALIVE	0x0008	/* keep connections alive */
#define	SO_DONTROUTE	0x0010	/* just use interface addresses */
#define	SO_BROADCAST	0x0020	/* permit sending of broadcast msgs */
#define	SO_USELOOPBACK	0x0040	/* bypass hardware when possible */
#define	SO_LINGER		0x0080	/* linger on close if data present */
#define	SO_OOBINLINE	0x0100	/* leave received OOB data in line */
#define	SO_REUSEPORT	0x0200	/* allow local address & port reuse */

/*
 * Additional options, not kept in so_options.
 */
#define SO_SNDBUF	0x1001		/* send buffer size */
#define SO_RCVBUF	0x1002		/* receive buffer size */
#define SO_SNDLOWAT	0x1003		/* send low-water mark */
#define SO_RCVLOWAT	0x1004		/* receive low-water mark */
#define SO_SNDTIMEO	0x1005		/* send timeout */
#define SO_RCVTIMEO	0x1006		/* receive timeout */
#define	SO_ERROR	0x1007		/* get error status and clear */
#define	SO_TYPE		0x1008		/* get socket type */

/* This is a private option which is used exclusively
 * by this Amiga TCP/IP stack implementation and should not
 * be used by user code.
 */
#define SO_EVENTMASK	0x2001

/*
 * Structure used for manipulating linger optio *
 */
struct linger
{
	int	l_onoff;	/* option on/off */
	int	l_linger;	/* linger time in seconds */
};

/*
 * Level number for (get/set)sockopt() to apply to socket itself.
 */
#define	SOL_SOCKET	0xffff	/* options for socket level */

#define SOL_IP          0
#define SOL_IPV6        41
#define SOL_ICMPV6      58

/*
 * Address families.
 */
#define	AF_UNSPEC		0			/* unspecified */
#define	AF_LOCAL		1 	        /* local to host (pipes, portals) */
#define	AF_UNIX			AF_LOCAL    /* backward compatibility */
#define	AF_INET			2			/* internetwork: UDP, TCP, etc. */
#define AF_INET6		10			/* IP version 6	 */
#define	AF_IMPLINK		3			/* arpanet imp addresses */
#define	AF_PUP			4			/* pup protocols: e.g. BSP */
#define	AF_CHAOS		5			/* mit CHAOS protocols */
#define	AF_NS			6			/* XEROX NS protocols */
#define	AF_ISO			7			/* ISO protocols */
#define	AF_OSI			AF_ISO
#define	AF_ECMA			8			/* european computer manufacturers */
#define	AF_DATAKIT		9			/* datakit protocols */
#define	AF_CCITT		10			/* CCITT protocols, X.25 etc */
#define	AF_SNA			11			/* IBM SNA */
#define AF_DECnet		12			/* DECnet */
#define AF_DLI			13			/* DEC Direct data link interface */
#define AF_LAT			14			/* LAT */
#define	AF_HYLINK		15			/* NSC Hyperchannel */
#define	AF_APPLETALK	16			/* Apple Talk */
#define	AF_ROUTE		17			/* Internal Routing Protocol */
#define	AF_LINK			18			/* Link layer interface */
#define	pseudo_AF_XTP	19			/* eXpress Transfer Protocol (no AF) */
#define	AF_COIP			20			/* connection-oriented IP, aka ST II */
#define	AF_CNT			21			/* Computer Network Technology */
#define pseudo_AF_RTIP	22			/* Help Identify RTIP packets */
#define	AF_IPX			23			/* Novell Internet Protocol */
#define	AF_SIP			24			/* Simple Internet Protocol */
#define pseudo_AF_PIP	25			/* Help Identify PIP packets */

#define	AF_MAX			26

/*
 * Structure used by kernel to store most
 * addresses.
 */
struct sockaddr
{
	unsigned char sa_len;		/* total length */
	unsigned char sa_family;	/* address family */
	unsigned char sa_data[14];	/* actually longer; address value */
};

/* Structure large enough to hold any socket address (with the historical exception of AF_UNIX).  */
#define _SS_MAXSIZE 128
#define _SS_ALIGNSIZE (sizeof(signed long long))
#define _SS_PAD1SIZE (_SS_ALIGNSIZE - sizeof (short))
#define _SS_PAD2SIZE (_SS_MAXSIZE - (sizeof (short) + _SS_PAD1SIZE + _SS_ALIGNSIZE))

struct sockaddr_storage {
        short ss_family;
        char __ss_pad1[_SS_PAD1SIZE];
        signed long long __ss_align;
        char __ss_pad2[_SS_PAD2SIZE];
};

/*
 * Structure used by kernel to pass protocol
 * inf * inf * inf * informati
 */
struct sockproto
{
	unsigned short	sp_family;		/* address family */
	unsigned short	sp_protocol;	/* protocol */
};

/*
 * Protocol families, same as address families for now.
 */
#define	PF_UNSPEC		AF_UNSPEC
#define	PF_LOCAL		AF_LOCAL
#define	PF_UNIX			PF_LOCAL	/* backward compatibility */
#define	PF_INET			AF_INET
#define PF_INET6		AF_INET6
#define	PF_IMPLINK		AF_IMPLINK
#define	PF_PUP			AF_PUP
#define	PF_CHAOS		AF_CHAOS
#define	PF_NS			AF_NS
#define	PF_ISO			AF_ISO
#define	PF_OSI			AF_ISO
#define	PF_ECMA			AF_ECMA
#define	PF_DATAKIT		AF_DATAKIT
#define	PF_CCITT		AF_CCITT
#define	PF_SNA			AF_SNA
#define PF_DECnet		AF_DECnet
#define PF_DLI			AF_DLI
#define PF_LAT			AF_LAT
#define	PF_HYLINK		AF_HYLINK
#define	PF_APPLETALK	AF_APPLETALK
#define	PF_ROUTE		AF_ROUTE
#define	PF_LINK			AF_LINK
#define	PF_XTP			pseudo_AF_XTP	/* really just proto family, no AF */
#define	PF_COIP			AF_COIP
#define	PF_CNT			AF_CNT
#define	PF_SIP			AF_SIP
#define	PF_IPX			AF_IPX			/* same format as AF_NS */
#define PF_RTIP			pseudo_AF_FTIP	/* same format as AF_INET */
#define PF_PIP			pseudo_AF_PIP

#define	PF_MAX			AF_MAX

/*
 * Definitions for network related sysctl, CTL_NET.
 *
 * Second level is protocol family.
 * Third level is protocol number.
 *
 * Further levels are defined by the individual families below.
 */
#define NET_MAXID	AF_MAX

/*
 * PF_ROUTE - Routing table
 *
 * Three additional levels are defined:
 *	Fourth: address family, 0 is wildcard
 *	Fifth: type of info, defined below
 *	Sixth: flag(s) to mask with for NET_RT_FLAGS
 */
#define NET_RT_DUMP		1	/* dump; may limit to a.f. */
#define NET_RT_FLAGS	2	/* by flags, e.g. RESOLVING */
#define NET_RT_IFLIST	3	/* survey interface list */
#define	NET_RT_MAXID	4

/*
 * Maximum queue length specifiable by listen.
 */
#define	SOMAXCONN	5

/*
 * Message header for recvmsg and sendmsg calls.
 * Used value-result for recvmsg, value only for sendmsg.
 */
struct msghdr
{
	void *			msg_name;		/* optional address */
	unsigned long	msg_namelen;	/* size of address */
	struct iovec *	msg_iov;		/* scatter/gather array */
	unsigned long 	msg_iovlen;		/* # elements in msg_iov */
	void *			msg_control;	/* ancillary data, see below */
	unsigned long	msg_controllen;	/* ancillary data buffer len */
	int				msg_flags;		/* flags on received message */
};

#define	MSG_OOB			0x1		/* process out-of-band data */
#define	MSG_PEEK		0x2		/* peek at incoming message */
#define	MSG_DONTROUTE	0x4		/* send without using routing tables */
#define	MSG_EOR			0x8		/* data completes record */
#define	MSG_TRUNC		0x10	/* data discarded before delivery */
#define	MSG_CTRUNC		0x20	/* control data lost before delivery */
#define	MSG_WAITALL		0x40	/* wait for full request or error */
#define	MSG_DONTWAIT	0x80	/* this message should be nonblocking */
#define MSG_NOSIGNAL    0x4000
/*
 * Header for ancillary data objects in msg_control buffer.
 * Used for additional information with/about a datagram
 * not expressible by flags.  The format is a sequence
 * of message elements headed by cmsghdr structures.
 */
struct cmsghdr
{
	unsigned long	cmsg_len;		/* data byte count, including hdr */
	int				cmsg_level;		/* originating protocol */
	int				cmsg_type;		/* protocol-specific type */
/* followed by	unsigned char cmsg_data[]; */
};

/* given pointer to struct cmsghdr, return pointer to data */
#define	CMSG_DATA(cmsg)		((unsigned char *)((cmsg) + 1))

/* given pointer to struct cmsghdr, return pointer to next cmsghdr */
#define	CMSG_NXTHDR(mhdr, cmsg)	\
	(((void *)(cmsg) + (cmsg)->cmsg_len + sizeof(struct cmsghdr) > \
	    (mhdr)->msg_control + (mhdr)->msg_controllen) ? \
	    (struct cmsghdr *)NULL : \
	    (struct cmsghdr *)((void *)(cmsg) + CMSG_ALIGN((cmsg)->cmsg_len)))

#define	CMSG_FIRSTHDR(mhdr)	((struct cmsghdr *)(mhdr)->msg_control)

#define CMSG_ALIGN(len) (((len) + sizeof (size_t) - 1) \
			 & (size_t) ~(sizeof (size_t) - 1))
#define CMSG_SPACE(len) (CMSG_ALIGN (len) \
			 + CMSG_ALIGN (sizeof (struct cmsghdr)))
#define CMSG_LEN(len)   (CMSG_ALIGN (sizeof (struct cmsghdr)) + (len))

/* "Socket"-level control message types: */
#define	SCM_RIGHTS	0x01		/* access rights (array of LONG) */

/* The following constants should be used for the second parameter of `shutdown'.  */
enum
{
  SHUT_RD = 0,          /* No more receptions.  */
#define SHUT_RD         SHUT_RD
  SHUT_WR,              /* No more transmissions.  */
#define SHUT_WR         SHUT_WR
  SHUT_RDWR             /* No more receptions or transmissions.  */
#define SHUT_RDWR       SHUT_RDWR
};

extern int accept(int sockfd,struct sockaddr *cliaddr,socklen_t *addrlen);
extern int bind(int socket, const struct sockaddr *address, socklen_t address_len);
extern int connect(int socket, const struct sockaddr *address, socklen_t address_len);
extern int getpeername(int socket, struct sockaddr *address, socklen_t *address_len);
extern int getsockname(int socket, struct sockaddr *address, socklen_t *address_len);
extern int getsockopt(int socket, int level, int option_name, void *option_value, socklen_t *option_len);
extern int listen(int socket, int backlog);
extern ssize_t recv(int socket, void *buffer, size_t length, int flags);
extern ssize_t recvfrom(int socket, void *buffer, size_t length,int flags, struct sockaddr *address,socklen_t *address_len);
extern ssize_t recvmsg(int socket, struct msghdr *message, int flags);
extern ssize_t send(int socket, const void *buffer, size_t length, int flags);
extern ssize_t sendmsg(int socket, const struct msghdr *message, int flags);
extern ssize_t sendto(int socket, const void *message, size_t length,int flags, const struct sockaddr *dest_addr,socklen_t dest_len);
extern int setsockopt(int socket, int level, int option_name, const void *option_value, socklen_t option_len);
extern int shutdown(int socket, int how);
extern int socket(int domain, int type, int protocol);
extern int socketpair (int domain, int type, int protocol, int fds[2]);

#ifdef __GNUC__
 #ifdef __PPC__
  #pragma pack()
 #endif
#elif defined(__VBCC__)
 #pragma default-align
#endif

__END_DECLS

#endif /* _SYS_SOCKET_H */
