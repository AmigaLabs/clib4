#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include "lookup.h"

int __lookup_ipliteral(struct address buf[static 1], const char *name, int family) {
    struct in_addr a4;
    if (inet_aton(name, &a4) > 0) {
        if (family == AF_INET6) /* wrong family */
            return EAI_NONAME;
        memcpy(&buf[0].addr, &a4, sizeof a4);
        buf[0].family = AF_INET;
        buf[0].scopeid = 0;
        return 1;
    }

    return EAI_NONAME;
}
