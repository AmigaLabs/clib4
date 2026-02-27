#define _GNU_SOURCE

#include <stdio.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    int i;
    struct hostent *he;
    struct in_addr **addr_list;
    struct in_addr addr;

    // get the addresses of www.yahoo.com:
    he = gethostbyname("www.yahoo.com");
    if (he == NULL) { // do some error checking
        herror("gethostbyname"); // herror(), NOT perror()
        exit(1);
    }

    // print information about this host:
    printf("Official name is: %s\n", he->h_name);
    printf("IP address: %s\n", inet_ntoa(*(struct in_addr *) he->h_addr));
    printf("All addresses: ");
    addr_list = (struct in_addr **) he->h_addr_list;
    for (i = 0; addr_list[i] != NULL; i++) {
        char *host = inet_ntoa(*addr_list[i]);
        printf("%s ", host);
    }
    printf("\n");

    printf("Reversed Addresses: \n");
    for (i = 0; addr_list[i] != NULL; i++) {
        char *host = inet_ntoa(*addr_list[i]);
        if (inet_aton(host, &addr) == 1) {
            he = gethostbyaddr(&addr, sizeof(addr), AF_INET);
            if (he)
                printf("Host name: %s\n", he->h_name);
            else
                printf("Cannot resolve host by IP for host %s\n", host);

        } else {
            printf("Cannot resolve host name for IP %s\n", addr_list[i]);
        }
    }

    return 0;
}