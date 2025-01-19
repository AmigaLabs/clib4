/*
 * $Id: getifaddrs.c,v 1.0 2022-02-10 17:02:58 clib4devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

#include <ifaddrs.h>
#include <net/if.h>

#include <proto/bsdsocket.h>

#define ROUNDUP(x, size)    ((((x) + (size) - 1) / (size)) * (size))
#define IFCONF_STARTENT 10
#define IFCONF_MAXENT 1000

struct ifawrap {
    struct ifaddrs *ifaddrs;
    struct ifaddrs *prev;
};

static int
ifaddrs_add(struct ifawrap *ifawrap, char *name, unsigned int flags,
            struct sockaddr *addr, struct sockaddr *netmask,
            struct sockaddr *dstaddr, struct sockaddr *data, size_t addrlen) {
    size_t nameoff, addroff, maskoff, dstoff;

    struct ifaddrs *new;
    size_t addrskip;
    size_t namelen;
    size_t nsize;
    char *p;

    namelen = strlen(name) + 1;

    addrskip = ROUNDUP(addrlen, sizeof(uint32_t));

    nsize = 0;
    nsize += ROUNDUP(sizeof(struct ifaddrs), sizeof(uint32_t));
    nameoff = nsize;

    nsize += ROUNDUP(namelen, sizeof(uint32_t));

    nsize += ROUNDUP(sizeof(flags), sizeof(uint32_t));
    addroff = nsize;

    if (addr != NULL)
        nsize += addrskip;
    maskoff = nsize;

    if (netmask != NULL)
        nsize += addrskip;
    dstoff = nsize;

    if (dstaddr != NULL)
        nsize += addrskip;

    if (data != NULL) /*XXX*/
        nsize += addrskip; /*XXX*/

    if ((new = malloc(nsize)) == NULL)
        return -1; /* let caller free already allocated data */

    if (ifawrap->ifaddrs == NULL || ifawrap->prev == NULL) {
        ifawrap->ifaddrs = new;
    }
    else {
        ifawrap->prev->ifa_next = new;
    }

    ifawrap->prev = new;

    new->ifa_next = NULL;

    p = (char *) new + nameoff;

    strncpy(p, name, namelen - 1);
    p[namelen - 1] = '\0';
    new->ifa_name = p;

    new->ifa_flags = flags;

    if (addr != NULL) {
        p = (char *) new + addroff;
        memcpy(p, addr, addrlen);
        new->ifa_addr = (struct sockaddr *) p;
    } else
        new->ifa_addr = NULL;

    if (netmask != NULL) {
        p = (char *) new + maskoff;
        memcpy(p, netmask, addrlen);
        new->ifa_netmask = (struct sockaddr *) p;
    } else
        new->ifa_netmask = NULL;

    if (dstaddr != NULL) {
        p = (char *) new + dstoff;
        memcpy(p, dstaddr, addrlen);
        new->ifa_dstaddr = (struct sockaddr *) p;
    } else
        new->ifa_dstaddr = NULL;

    new->ifa_data = NULL;

    return 0;
}

int
getifaddrs(struct ifaddrs **ifap) {
    unsigned int success = 0;

    struct List *netiflist = NULL;
    struct Node *node = NULL;
    struct ifawrap _ifawrap, *ifawrap = &_ifawrap;
    size_t addrlen;

    DECLARE_SOCKETBASE();

    memset(ifawrap, 0, sizeof(*ifawrap));

    netiflist = ObtainInterfaceList();
    if (netiflist != NULL) {
        node = GetHead(netiflist);
        if (node != NULL) {
            while (node != NULL) {
                if (node->ln_Name != NULL) {
                    UBYTE hwAddress[IF_NAMESIZE] = {0};
                    LONG mtu, metric, state, bindType, index, debug;
                    ULONG packetsReceived, packetsSent, badData, overruns;
                    struct sockaddr localAddress, destinationAddress, broadcastAddress;
                    struct sockaddr_in netmask, primaryDns, secondaryDns;
                    unsigned int flags = 0;
                    // FOUND ONE
                    long querySuccess = QueryInterfaceTags(node->ln_Name,
                                                         IFQ_HardwareAddress, &hwAddress,
                                                         IFQ_HardwareMTU, &mtu,
                                                         IFQ_DeviceUnit, &index,
                                                         IFQ_PacketsReceived, &packetsReceived,
                                                         IFQ_PacketsSent, &packetsSent,
                                                         IFQ_BadData, &badData,
                                                         IFQ_Overruns, &overruns,
                                                         IFQ_Address, &localAddress,
                                                         IFQ_DestinationAddress, &destinationAddress,
                                                         IFQ_BroadcastAddress, &broadcastAddress,
                                                         IFQ_NetMask, &netmask,
                                                         IFQ_Metric, &metric,
                                                         IFQ_State, &state,
                                                         IFQ_GetDebugMode, &debug,
                                                         IFQ_AddressBindType, &bindType,
                                                         IFQ_PrimaryDNSAddress, &primaryDns,
                                                         IFQ_SecondaryDNSAddress, &secondaryDns,
                                                         TAG_DONE);
                    if (querySuccess) {
                        addrlen = sizeof(struct sockaddr);
                        /* TODO - Move this to ioctl with SIOCGIFFLAGS request */
                        if (state == SM_Online)
                            flags |= IFF_UP;
                        if (state == SM_Up)
                            flags |= IFF_RUNNING;
                        if (debug == TRUE)
                            flags |= IFF_DEBUG;

                        if (ifaddrs_add(ifawrap, node->ln_Name, flags, &localAddress, (struct sockaddr*) &netmask, &broadcastAddress, NULL, addrlen) == -1) {
                            if (ifawrap->ifaddrs != NULL) {
                                freeifaddrs(ifawrap->ifaddrs);
                                success = -1;
                                break;
                            }
                        }
                    } else {
                        success = -1;
                        break;
                    }
                }
                node = GetSucc(node);
            }
        }
        ReleaseInterfaceList(netiflist);
    }

    if (success == 0 && ifawrap->ifaddrs != NULL)
        *ifap = ifawrap->ifaddrs;

    __check_abort();

    return success;
}