/*
 * $Id: if_if_nametoindex.c,v 1.0 2021-01-24 18:59:58 clib2devs Exp $
*/

#ifndef _UNISTD_HEADERS_H
#include "unistd_headers.h"
#endif /* _UNISTD_HEADERS_H */

#ifndef _SOCKET_HEADERS_H
#include "socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

#include <net/if.h>
#include <proto/bsdsocket.h>

unsigned int
if_nametoindex(const char *ifname) {
    unsigned int index = 1;

    struct List *netiflist = NULL;
    struct Node *node = NULL;
    DECLARE_SOCKETBASE();

    ENTER();
    SHOWSTRING(ifname);

    netiflist = ObtainInterfaceList();
    if (netiflist != NULL) {
        node = GetHead(netiflist);
        while (node != NULL) {
            if (node->ln_Name != NULL) {
                if (strcasecmp(node->ln_Name, ifname) == SAME) {
                    // Found our interface
                    goto out;
                }
            }
            index++;
            node = GetSucc(node);
        }
        ReleaseInterfaceList(netiflist);
    }
    /* We did not found the interface. Set index to ZERO */
    index = 0;

out:
    __check_abort();

    RETURN(index);
    return index;
}