#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    struct servent *sptr = getservbyname("tcpmux", "tcp");

    if (!sptr) {
        printf("No entry found for this service.\n");
        exit(1);
    }

    printf("service name is %s.\n"
           "service port is %d.\n"
           "protocol to use is %s.\n",
           sptr->s_name, sptr->s_port, sptr->s_proto);

    return 0;
}