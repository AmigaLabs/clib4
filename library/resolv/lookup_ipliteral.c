#ifndef _SOCKET_HEADERS_H
#include "../socket/socket_headers.h"
#endif /* _SOCKET_HEADERS_H */

#ifndef _STRING_HEADERS_H
#include "string_headers.h"
#endif /* _STRING_HEADERS_H */

#ifndef _TIME_HEADERS_H
#include "time_headers.h"
#endif /* _TIME_HEADERS_H */

#include "lookup.h"

int
__lookup_ipliteral(struct address buf[static 1], const char *name, int family) {
    (void) (family);
    struct in_addr a4;
    if (inet_aton(name, &a4) > 0) {
        memcpy(&buf[0].addr, &a4, sizeof a4);
        buf[0].family = AF_INET;
        buf[0].scopeid = 0;
        return 1;
    }

    return EAI_NONAME;
}
