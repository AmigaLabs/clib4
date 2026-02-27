static char rcsid[] =
        "$Id: ifconfig.c,v 3.1 1994/02/22 03:28:22 ppessi Exp $";
/*
 * ifconfig.c --- Configure Network Interfaces
 *
 * Author: Pekka Pessi <Pekka.Pessi@hut.fi>
 *
 * Copyright © 1993 AmiTCP/IP Group, <amitcp-group@hut.fi>
 *		    Helsinki University of Technology, Finland.
 *		    All rights reserved.
 *
 * Created      : Sat May 15 04:12:15 1993 ppessi
 * Last modified: Tue Feb 22 01:35:17 1994 ppessi
 */

static const char version[] = "\0$VER: netstat 1.0 (08.08.2022)"
" "
"Copyright © 1993 AmiTCP/IP Group, <amitcp-group@hut.fi>\n"
"Helsinki University of Technology, Finland.\n"
"All rights reserved.\n"
"@(#) Copyright © 1983 The Regents of the University of California.\n"
"All rights reserved.\n";

/****** netutil.doc/ifconfig ************************************************

   NAME
        ifconfig - configure network interface parameters

   VERSION
        $Id: ifconfig.c,v 3.1 1994/02/22 03:28:22 ppessi Exp $

   SYNOPSIS
        ifconfig interface address_family [address [dest_address]] [params]
        ifconfig interface [address_family]

   DESCRIPTION
        ifconfig is used to assign an address to a network interface and/or
        configure network interface parameters.  ifconfig must be used at
        boot time to define the network address of each interface present on
        a machine.  It can also be used at other times to redefine an
        interface's address or other operating parameters.

   PARAMETERS
        interface     A string of the interface name concatenated with unit
                      numver, for example `eth0'.  The AmiTCP/IP network
                      interfaces are defined in the `AmiTCP:db/interface'
                      file.  For example, a interface sl corresponds by
                      default to `Devs:networks/rhcslip.device'.

        address_family

                      Name of protocol on which naming scheme is based.  An
                      interface can receive transmissions in differing
                      protocols, each of which may require separate naming
                      schemes.  Therefore, it is necessary to specify the
                      address_family, which may affect interpretation of the
                      remaining parameters on the command line.  The only
                      address family currently supported is inet (DARPA-
                      Internet family).

        address       Either a host name present in the host name database,
                      (SEE hosts), or a DARPA Internet address
                      expressed in Internet standard "dot notation".  The
                      host number can be omitted on 10-Mbyte/second Ethernet
                      interfaces (which use the hardware physical address),
                      and on interfaces other than the first.

        dest_address  Address of destination system.  Consists of either a
                      host name present in the host name database, hosts(4),
                      or a DARPA Internet address expressed in Internet
                      standard "dot notation".

   SWITCHES
        The following operating parameters can be specified:

         up           Mark an interface "up". Enables interface after an
                      "ifconfig down."  Occurs automatically when setting the
                      address on an interface.  Setting this flag has no
                      effect if the hardware is "down".

         down         Mark an interface "down".  When an interface is marked
                      "down", the system will not attempt to transmit
                      messages through that interface. If possible, the
                      interface will be reset to disable reception as well.
                      This action does not automatically disable routes
                      using the interface.

        arp           Enable the use of Address Resolution Protocol in
                      mapping between network level addresses and link-level
                      addresses (default).

        -arp          Disable the use of Address Resolution Protocol.

         metric n     Set the routing metric of the interface to n, default
                      0.  The routing metric is used by the routing protocol
                      (see gated).  Higher metrics have the effect of making
                      a route less favorable; metrics are counted as
                      additional hops to the destination network or host.

         debug        Enable driver-dependent debugging code. This usually
                      turns on extra console error logging.

        -debug        Disable driver-dependent debugging code.

         netmask mask (Inet only) Specify how much of the address to reserve
                      for subdividing networks into sub-networks.  mask
                      includes the network part of the local address, and
                      the subnet part which is taken from the host field of
                      the address.  mask can be specified as a single hexa-
                      decimal number with a leading 0x, with a dot-notation
                      Internet address, or with a pseudo-network name listed
                      in the file AmiTCP:db/networks.  `mask' contains 1's
                      for each bit position in the 32-bit address that are
                      to be used for the network and subnet parts, and 0's
                      for the host part.  mask should contain at least the
                      standard network portion, and the subnet field should
                      be contiguous with the network portion.

        broadcast    (Inet only) Specify the address that represents
                      broadcasts to the network.  The default broadcast
                      address is the address with a host part of all 1's.

        The command:

             ifconfig interface/unit

        with no optional command arguments supplied displays the current
        configuration for interface.  If address_family is specified,
        ifconfig reports only the details specific to that address family.

   DIAGNOSTICS
        Messages indicating that the specified interface does not exist, the
        requested address is unknown, or the user is not privileged and
        tried to alter an interface's configuration.

   EXAMPLES
        ifconfig lo0 127.0.0.1

                This command marks internal loopback device "UP", and
                attach an inet address 127.0.0.1 to it.

        ifconfig cslip0 inet 193.102.4.144 193.102.4.129

                This command starts the CSLIP driver, attach an
                address 193.102.4.144 (our internet address) and a
                destination address 193.102.4.129 (the internet
                address of the host you are connecting) to it.

        ifconfig eth0 inet 193.124.100.64 netmask 255.255.255.192 -arp

                This command loads an ethernet driver (by default for the
                Commodore A2065 Ethernet adapter unit 0), marks it "up",
                disables ARP protocol for it, attaches an inet address
                193.124.100.65 to it and sets its netmask to
                255.255.255.192.  A bitwise logical and of netmask and
                address for the interface forms a subnet address, in this
                case 193.124.100.64.  All packets aimed to hosts with same
                subnet address (that is hosts 193.124.100.64 -
                193.124.100.127) are routed to this interface.

   FILES
        AmiTCP:db/interfaces

   SEE ALSO
        netstat, hosts, arp, "net/if.h", "net/sana2tags.h"

*****************************************************************************
*
*/

/*
 * Copyright © 1983 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <sys/param.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <sys/sockio.h>

#ifdef NS
#define	NSIP
#include <netns/ns.h>
#include <netns/ns_if.h>
#endif

#include <netdb.h>

#ifdef ISO
#define EON
#include <netiso/iso.h>
#include <netiso/iso_var.h>
#endif

#ifdef notyet
#include <unistd.h>
#endif

#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

struct ifreq ifr, ridreq;
struct ifaliasreq addreq;
#ifdef ISO
struct	iso_ifreq	iso_ridreq;
struct	iso_aliasreq	iso_addreq;
#endif
struct sockaddr_in netmask;

char name[1024];
int flags;
int metric;
int setaddr;
int setipdst;
int doalias;
int clearaddr;
int newaddr = 1;
int s;
extern int errno;

#ifdef NS
int	nsellength = 1;
#endif

void status(void);
void printb(char *s, register unsigned short v, register char *bits);
void setifflags(char *, short);
void setifaddr(char *, short);
void setifdstaddr(char *, short);
void setifnetmask(char *, short);
void setifmetric(char *, short);
void setifbroadaddr(char *, short);
void setifipdst(char *, short);
void notealias(char *, short);
void setsnpaoffset(char *, short);
void setnsellength(char *, short);

#define    NEXTARG        0xffffff

struct cmd {
    char *c_name;
    int c_parameter;        /* NEXTARG means next argv */
    void (*c_func)(char *, short);
} cmds[] = {
        {"up", IFF_UP, setifflags},
        {"down", -IFF_UP, setifflags},
        {"trailers", -IFF_NOTRAILERS, setifflags},
        {"-trailers", IFF_NOTRAILERS, setifflags},
        {"arp", -IFF_NOARP, setifflags},
        {"-arp", IFF_NOARP, setifflags},
        {"debug", IFF_DEBUG, setifflags},
        {"-debug", -IFF_DEBUG, setifflags},
        {"alias", IFF_UP, notealias},
        {"-alias", -IFF_UP, notealias},
        {"delete", -IFF_UP, notealias},
#ifdef notdef
#define	EN_SWABIPS	0x1000
        { "swabips",	EN_SWABIPS,	setifflags },
        { "-swabips",	-EN_SWABIPS,	setifflags },
#endif
        {"netmask", NEXTARG, setifnetmask},
        {"metric", NEXTARG, setifmetric},
        {"broadcast", NEXTARG, setifbroadaddr},
        {"ipdst", NEXTARG, setifipdst},
#ifdef ISO
        { "snpaoffset",	NEXTARG,	setsnpaoffset },
    { "nsellength",	NEXTARG,	setnsellength },
#endif
        {0, 0, setifaddr},
        {0, 0, setifdstaddr},
};

void in_status(int);

void in_getaddr(char *addr, int which);

#ifdef NS
/*
 * XNS support liberally adapted from
 * code written at the University of Maryland
 * principally by James O'Toole and Chris Torek.
 */
void xns_status(int);
void xns_getaddr(char *addr, int which);
#endif
#ifdef ISO
void iso_status(int);
void iso_getaddr(char *addr, int which);
#endif

/* Known address families */
struct afswtch {
    char *af_name;
    short af_af;

    void (*af_status)(int);

    void (*af_getaddr)(char *, int);

    int af_difaddr;
    int af_aifaddr;
    caddr_t af_ridreq;
    caddr_t af_addreq;
} afs[] = {
#define C(x) ((caddr_t) &x)
        {"inet", AF_INET, in_status, in_getaddr,
         SIOCDIFADDR, SIOCAIFADDR, C(ridreq), C(addreq)},
#ifdef NS
        { "ns", AF_NS, xns_status, xns_getaddr,
         SIOCDIFADDR, SIOCAIFADDR, C(ridreq), C(addreq) },
#endif
#ifdef ISO
        { "iso", AF_ISO, iso_status, iso_getaddr,
         SIOCDIFADDR_ISO, SIOCAIFADDR_ISO, C(iso_ridreq), C(iso_addreq) },
#endif
        {0, 0, 0, 0}
};

struct afswtch *afp;    /*the address family being set or asked about*/

int main(int argc, char *argv[]) {
    int af = AF_INET;
    register struct afswtch *rafp;

    if (argc < 2) {
        fprintf(stderr, "usage: ifconfig interface\n%s%s%s%s%s",
                "\t[ [ af ] [ address [ dest_addr ] ] [ up ] [ down ]",
                "[ netmask mask ] ]\n",
                "\t[ metric n ]\n",
                "\t[ trailers | -trailers ]\n",
                "\t[ arp | -arp ]\n");
        exit(1);
    }
    argc--, argv++;
    strncpy(name, *argv, sizeof(name));
    strncpy(ifr.ifr_name, name, sizeof(ifr.ifr_name));
    argc--, argv++;
    if (argc > 0) {
        for (afp = rafp = afs; rafp->af_name; rafp++)
            if (strcmp(rafp->af_name, *argv) == 0) {
                afp = rafp;
                argc--;
                argv++;
                break;
            }
        rafp = afp;
        af = ifr.ifr_addr.sa_family = rafp->af_af;
    }
    s = socket(af, SOCK_DGRAM, 0);
    if (s < 0) {
        perror("ifconfig: socket");
        exit(1);
    }
    if (ioctl(s, SIOCGIFFLAGS, (caddr_t) & ifr) < 0) {
        perror("ioctl (SIOCGIFFLAGS)");
        exit(1);
    }
    strncpy(ifr.ifr_name, name, sizeof ifr.ifr_name);
    flags = ifr.ifr_flags;
    if (ioctl(s, SIOCGIFMETRIC, (caddr_t) & ifr) < 0)
        perror("ioctl (SIOCGIFMETRIC)");
    else
        metric = ifr.ifr_metric;
    if (argc == 0) {
        status();
        exit(0);
    }
    while (argc > 0) {
        register struct cmd *p;

        for (p = cmds; p->c_name; p++)
            if (strcmp(*argv, p->c_name) == 0)
                break;
        if (p->c_name == 0 && setaddr)
            p++;    /* got src, do dst */
        if (p->c_func) {
            if (p->c_parameter == NEXTARG) {
                (*p->c_func)(argv[1], 0);
                argc--, argv++;
            } else
                (*p->c_func)(*argv, p->c_parameter);
        }
        argc--, argv++;
    }
#ifdef ISO
    if (af == AF_ISO)
        adjust_nsellength();
#endif
#ifdef NS
    if (setipdst && af==AF_NS) {
        struct nsip_req rq;
        int size = sizeof(rq);

        rq.rq_ns = addreq.ifra_addr;
        rq.rq_ip = addreq.ifra_dstaddr;

        if (setsockopt(s, 0, SO_NSIP_ROUTE, &rq, size) < 0)
            perror("Encapsulation Routing");
    }
#endif
    if (clearaddr) {
        int ret;
        strncpy(rafp->af_ridreq, name, sizeof ifr.ifr_name);
        if ((ret = ioctl(s, rafp->af_difaddr, rafp->af_ridreq)) < 0) {
            if (errno == EADDRNOTAVAIL && (doalias >= 0)) {
                /* means no previous address for interface */
            } else
                perror("ioctl (SIOCDIFADDR)");
        }
    }
    if (newaddr) {
        strncpy(rafp->af_addreq, name, sizeof ifr.ifr_name);
        if (ioctl(s, rafp->af_aifaddr, rafp->af_addreq) < 0)
            perror("ioctl (SIOCAIFADDR)");
    }
    exit(0);
}
#define RIDADDR 0
#define ADDR    1
#define MASK    2
#define DSTADDR    3

/*ARGSUSED*/
void
setifaddr(char *addr, short param) {
    /*
     * Delay the ioctl to set the interface addr until flags are all set.
     * The address interpretation may depend on the flags,
     * and the flags may change when the address is set.
     */
    setaddr++;
    if (doalias == 0)
        clearaddr = 1;
    (*afp->af_getaddr)(addr, (doalias >= 0 ? ADDR : RIDADDR));
}

void
setifnetmask(char *addr, short dummy) {
    (*afp->af_getaddr)(addr, MASK);
}

void
setifbroadaddr(char *addr, short summy) {
    (*afp->af_getaddr)(addr, DSTADDR);
}

void
setifipdst(char *addr, short dummy) {
    in_getaddr(addr, DSTADDR);
    setipdst++;
    clearaddr = 0;
    newaddr = 0;
}

#define rqtosa(x) (&(((struct ifreq *)(afp->x))->ifr_addr))

void
notealias(char *addr, short param) {
    if (setaddr && doalias == 0 && param < 0)
        bcopy((caddr_t)rqtosa(af_addreq),
              (caddr_t)rqtosa(af_ridreq),
              rqtosa(af_addreq)->sa_len);
    doalias = param;
    if (param < 0) {
        clearaddr = 1;
        newaddr = 0;
    } else
        clearaddr = 0;
}

/*ARGSUSED*/
void
setifdstaddr(char *addr, short param) {
    (*afp->af_getaddr)(addr, DSTADDR);
}

void
setifflags(char *vname, short value) {
    if (ioctl(s, SIOCGIFFLAGS, (caddr_t) & ifr) < 0) {
        perror("ioctl (SIOCGIFFLAGS)");
        exit(1);
    }
    strncpy(ifr.ifr_name, name, sizeof(ifr.ifr_name));
    flags = ifr.ifr_flags;

    if (value < 0) {
        value = -value;
        flags &= ~value;
    } else
        flags |= value;
    ifr.ifr_flags = flags;
    if (ioctl(s, SIOCSIFFLAGS, (caddr_t) & ifr) < 0)
        perror(vname);
}

void
setifmetric(char *val, short dummy) {
    strncpy(ifr.ifr_name, name, sizeof(ifr.ifr_name));
    ifr.ifr_metric = atoi(val);
    if (ioctl(s, SIOCSIFMETRIC, (caddr_t) & ifr) < 0)
        perror("ioctl (set metric)");
}

#ifdef ISO
void
setsnpaoffset(char *val, short dummy)
{
    iso_addreq.ifra_snpaoffset = atoi(val);
}
#endif
#define    IFFBITS \
"\020\1UP\2BROADCAST\3DEBUG\4LOOPBACK\5POINTOPOINT\6NOTRAILERS\7RUNNING\10NOARP\
"

/*
 * Print the status of the interface.  If an address family was
 * specified, show it and it only; otherwise, show them all.
 */
void
status(void) {
    register struct afswtch *p = afp;
    short af = ifr.ifr_addr.sa_family;

    printf("%s: ", name);
    printb("flags", flags, IFFBITS);
    if (metric)
        printf(" metric %d", metric);
    putchar('\n');
    if ((p = afp) != NULL) {
        (*p->af_status)(1);
    } else
        for (p = afs; p->af_name; p++) {
            ifr.ifr_addr.sa_family = p->af_af;
            (*p->af_status)(0);
        }
}

void
in_status(int force) {
    struct sockaddr_in *sin;

    strncpy(ifr.ifr_name, name, sizeof(ifr.ifr_name));
    if (ioctl(s, SIOCGIFADDR, (caddr_t) & ifr) < 0) {
        if (errno == EADDRNOTAVAIL || errno == EAFNOSUPPORT) {
            if (!force)
                return;
            bzero((char *) &ifr.ifr_addr, sizeof(ifr.ifr_addr));
        } else
            perror("ioctl (SIOCGIFADDR)");
    }
    sin = (struct sockaddr_in *) &ifr.ifr_addr;
    printf("\tinet %s ", inet_ntoa(sin->sin_addr));
    strncpy(ifr.ifr_name, name, sizeof(ifr.ifr_name));
    if (ioctl(s, SIOCGIFNETMASK, (caddr_t) & ifr) < 0) {
        if (errno != EADDRNOTAVAIL)
            perror("ioctl (SIOCGIFNETMASK)");
        bzero((char *) &ifr.ifr_addr, sizeof(ifr.ifr_addr));
    } else
        netmask.sin_addr =
                ((struct sockaddr_in *) &ifr.ifr_addr)->sin_addr;
    if (flags & IFF_POINTOPOINT) {
        if (ioctl(s, SIOCGIFDSTADDR, (caddr_t) & ifr) < 0) {
            if (errno == EADDRNOTAVAIL)
                bzero((char *) &ifr.ifr_addr, sizeof(ifr.ifr_addr));
            else
                perror("ioctl (SIOCGIFDSTADDR)");
        }
        strncpy(ifr.ifr_name, name, sizeof(ifr.ifr_name));
        sin = (struct sockaddr_in *) &ifr.ifr_dstaddr;
        printf("--> %s ", inet_ntoa(sin->sin_addr));
    }
    printf("netmask %x ", ntohl(netmask.sin_addr.s_addr));
    if (flags & IFF_BROADCAST) {
        if (ioctl(s, SIOCGIFBRDADDR, (caddr_t) & ifr) < 0) {
            if (errno == EADDRNOTAVAIL)
                bzero((char *) &ifr.ifr_addr, sizeof(ifr.ifr_addr));
            else
                perror("ioctl (SIOCGIFADDR)");
        }
        strncpy(ifr.ifr_name, name, sizeof(ifr.ifr_name));
        sin = (struct sockaddr_in *) &ifr.ifr_addr;
        if (sin->sin_addr.s_addr != 0)
            printf("broadcast %s", inet_ntoa(sin->sin_addr));
    }
    putchar('\n');
}

#ifdef NS
xns_status(force)
    int force;
{
    struct sockaddr_ns *sns;

    close(s);
    s = socket(AF_NS, SOCK_DGRAM, 0);
    if (s < 0) {
        if (errno == EPROTONOSUPPORT)
            return;
        perror("ifconfig: socket");
        exit(1);
    }
    if (ioctl(s, SIOCGIFADDR, (caddr_t)&ifr) < 0) {
        if (errno == EADDRNOTAVAIL || errno == EAFNOSUPPORT) {
            if (!force)
                return;
            bzero((char *)&ifr.ifr_addr, sizeof(ifr.ifr_addr));
        } else
            perror("ioctl (SIOCGIFADDR)");
    }
    strncpy(ifr.ifr_name, name, sizeof ifr.ifr_name);
    sns = (struct sockaddr_ns *)&ifr.ifr_addr;
    printf("\tns %s ", ns_ntoa(sns->sns_addr));
    if (flags & IFF_POINTOPOINT) { /* by W. Nesheim@Cornell */
        if (ioctl(s, SIOCGIFDSTADDR, (caddr_t)&ifr) < 0) {
            if (errno == EADDRNOTAVAIL)
                bzero((char *)&ifr.ifr_addr, sizeof(ifr.ifr_addr));
            else
                perror("ioctl (SIOCGIFDSTADDR)");
        }
        strncpy(ifr.ifr_name, name, sizeof (ifr.ifr_name));
        sns = (struct sockaddr_ns *)&ifr.ifr_dstaddr;
        printf("--> %s ", ns_ntoa(sns->sns_addr));
    }
    putchar('\n');
}
#endif
#ifdef ISO
iso_status(force)
    int force;
{
    struct sockaddr_iso *siso;
    struct iso_ifreq ifr;

    close(s);
    s = socket(AF_ISO, SOCK_DGRAM, 0);
    if (s < 0) {
        if (errno == EPROTONOSUPPORT)
            return;
        perror("ifconfig: socket");
        exit(1);
    }
    bzero((caddr_t)&ifr, sizeof(ifr));
    strncpy(ifr.ifr_name, name, sizeof(ifr.ifr_name));
    if (ioctl(s, SIOCGIFADDR_ISO, (caddr_t)&ifr) < 0) {
        if (errno == EADDRNOTAVAIL || errno == EAFNOSUPPORT) {
            if (!force)
                return;
            bzero((char *)&ifr.ifr_Addr, sizeof(ifr.ifr_Addr));
        } else {
            perror("ioctl (SIOCGIFADDR_ISO)");
            exit(1);
        }
    }
    strncpy(ifr.ifr_name, name, sizeof ifr.ifr_name);
    siso = &ifr.ifr_Addr;
    printf("\tiso %s ", iso_ntoa(&siso->siso_addr));
    if (ioctl(s, SIOCGIFNETMASK_ISO, (caddr_t)&ifr) < 0) {
        if (errno != EADDRNOTAVAIL)
            perror("ioctl (SIOCGIFNETMASK_ISO)");
    } else {
        printf(" netmask %s ", iso_ntoa(&siso->siso_addr));
    }
    if (flags & IFF_POINTOPOINT) {
        if (ioctl(s, SIOCGIFDSTADDR_ISO, (caddr_t)&ifr) < 0) {
            if (errno == EADDRNOTAVAIL)
                bzero((char *)&ifr.ifr_Addr, sizeof(ifr.ifr_Addr));
            else
                perror("ioctl (SIOCGIFDSTADDR_ISO)");
        }
        strncpy(ifr.ifr_name, name, sizeof (ifr.ifr_name));
        siso = &ifr.ifr_Addr;
        printf("--> %s ", iso_ntoa(&siso->siso_addr));
    }
    putchar('\n');
}
#endif

#define SIN(x) ((struct sockaddr_in *) &(x))
struct sockaddr_in *sintab[] = {
        SIN(ridreq.ifr_addr), SIN(addreq.ifra_addr),
        SIN(addreq.ifra_mask), SIN(addreq.ifra_broadaddr)};

void
in_getaddr(char *s, int which) {
    register struct sockaddr_in *sin = sintab[which];
    struct hostent *hp;
    struct netent *np;
    int val;

    sin->sin_len = sizeof(*sin);
    if (which != MASK)
        sin->sin_family = AF_INET;

    if ((val = inet_addr(s)) != -1)
        sin->sin_addr.s_addr = val;
    else if (hp = gethostbyname(s))
        bcopy(hp->h_addr, (char *) &sin->sin_addr, hp->h_length);
    else if (np = getnetbyname(s))
        sin->sin_addr = inet_makeaddr(np->n_net, INADDR_ANY);
    else {
        fprintf(stderr, "%s: bad value\n", s);
        exit(1);
    }
}

/*
 * Print a value a la the %b format of the kernel's printf
 */
void
printb(char *s, register unsigned short v, register char *bits) {
    register int i, any = 0;
    register char c;

    if (bits && *bits == 8)
        printf("%s=%o", s, v);
    else
        printf("%s=%x", s, v);
    bits++;
    if (bits) {
        putchar('<');
        while (i = *bits++) {
            if (v & (1 << (i - 1))) {
                if (any)
                    putchar(',');
                any = 1;
                for (; (c = *bits) > 32; bits++)
                    putchar(c);
            } else
                for (; *bits > 32; bits++);
        }
        putchar('>');
    }
}

#ifdef NS
#define SNS(x) ((struct sockaddr_ns *) &(x))
struct sockaddr_ns *snstab[] = {
SNS(ridreq.ifr_addr), SNS(addreq.ifra_addr),
SNS(addreq.ifra_mask), SNS(addreq.ifra_broadaddr)};

xns_getaddr(addr, which)
char *addr;
{
    struct sockaddr_ns *sns = snstab[which];
    struct ns_addr ns_addr();

    sns->sns_family = AF_NS;
    sns->sns_len = sizeof(*sns);
    sns->sns_addr = ns_addr(addr);
    if (which == MASK)
        printf("Attempt to set XNS netmask will be ineffectual\n");
}
#endif

#ifdef ISO
#define SISO(x) ((struct sockaddr_iso *) &(x))
struct sockaddr_iso *sisotab[] = {
SISO(iso_ridreq.ifr_Addr), SISO(iso_addreq.ifra_addr),
SISO(iso_addreq.ifra_mask), SISO(iso_addreq.ifra_dstaddr)};

iso_getaddr(addr, which)
char *addr;
{
    register struct sockaddr_iso *siso = sisotab[which];
    struct iso_addr *iso_addr();
    siso->siso_addr = *iso_addr(addr);

    if (which == MASK) {
        siso->siso_len = TSEL(siso) - (caddr_t)(siso);
        siso->siso_nlen = 0;
    } else {
        siso->siso_len = sizeof(*siso);
        siso->siso_family = AF_ISO;
    }
}

void
setnsellength(char *val, short dummy)
{
    nsellength = atoi(val);
    if (nsellength < 0) {
        fprintf(stderr, "Negative NSEL length is absurd\n");
        exit (1);
    }
    if (afp == 0 || afp->af_af != AF_ISO) {
        fprintf(stderr, "Setting NSEL length valid only for iso\n");
        exit (1);
    }
}

fixnsel(s)
register struct sockaddr_iso *s;
{
    if (s->siso_family == 0)
        return;
    s->siso_tlen = nsellength;
}

adjust_nsellength()
{
    fixnsel(sisotab[RIDADDR]);
    fixnsel(sisotab[ADDR]);
    fixnsel(sisotab[DSTADDR]);
}
#endif
