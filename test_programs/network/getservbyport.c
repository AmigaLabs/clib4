#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>

static char *port_to_service(int port, unsigned short proto) {
    struct servent *service;

    if (proto == IPPROTO_TCP && (service = getservbyport(htons(port), "tcp")) != NULL)
        return service->s_name;
    else if (proto == IPPROTO_UDP &&
             (service = getservbyport(htons(port), "udp")) != NULL)
        return service->s_name;
    else
        return (char *) NULL;
}

int main() {
    printf("Service associated with port 22/UDP is: %s\n", port_to_service(22, IPPROTO_UDP));
    return 0;
}