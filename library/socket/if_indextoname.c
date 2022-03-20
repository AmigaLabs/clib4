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


char *
if_indextoname (unsigned int ifindex, char *ifname)
{
    unsigned int index = 1;

    if (ifname == NULL || strlen(ifname) > IF_NAMESIZE) {
        __set_errno(ENXIO);
        return NULL;
    }

    struct List *netiflist = NULL;
    struct Node *node = NULL;

    netiflist = __ISocket->ObtainInterfaceList();
    if (netiflist != NULL)
    {
        node = GetHead(netiflist);
        if (node != NULL)
        {
            while (node != NULL)
            {
                if (node->ln_Name != NULL)
                {
                    if (index == ifindex)
                    {
                        // Found our interface
                        strncpy(ifname, node->ln_Name, IF_NAMESIZE);
                        return ifname;
                    }
                }
                index++;
                node = GetSucc(node);
            }
        }
        __ISocket->ReleaseInterfaceList(netiflist);
    }

    __set_errno(ENXIO);
    return NULL;
}