/*
 * $Id: if_if_indextoname.c,v 1.0 2021-01-24 19:34:26 clib2devs Exp $
*/

#include <net/if.h>

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

#include <proto/bsdsocket.h>

char *
if_indextoname(unsigned int ifindex, char *ifname) {
    unsigned int index = 1;
    char *result = NULL;
    DECLARE_SOCKETBASE();

    ENTER();
    SHOWVALUE(ifindex);

    if (ifname == NULL || strlen(ifname) > IF_NAMESIZE) {
        __set_errno(ENXIO);
        goto out;
    }

    struct List *netiflist = NULL;
    struct Node *node = NULL;

    netiflist = ObtainInterfaceList();
    if (netiflist != NULL) {
        node = GetHead(netiflist);
        while (node != NULL) {
            if (node->ln_Name != NULL) {
                if (index == ifindex) {
                    // Found our interface
                    strncpy(ifname, node->ln_Name, IF_NAMESIZE);
                    result = ifname;
                    goto out;
                }
            }
            index++;
            node = GetSucc(node);
        }
        ReleaseInterfaceList(netiflist);
    }

    __set_errno(ENXIO);

out:
    __check_abort();

    RETURN(result);
    return result;
}