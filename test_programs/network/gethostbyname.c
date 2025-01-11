#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#define _GNU_SOURCE
#include <netdb.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    int i;
    struct hostent *he;
    struct in_addr **addr_list;
    struct in_addr addr;

    if (argc != 2) {
        fprintf(stderr, "usage: %s hostname\n", argv[0]);
        return 1;
    }
    // get the addresses of www.yahoo.com:

    he = gethostbyname(argv[1]);
    if (he == NULL) {                    // do some error checking
        printf("gethostbyname error %s\n", hstrerror(h_errno)); // herror(), NOT perror()
        exit(1);
    }

    // print information about this host:
    printf("Official name is: %s\n", he->h_name);
    const char *ipaddr = inet_ntoa(*(struct in_addr *) he->h_addr);
    printf("IP address: %s\n", ipaddr);
    printf("All addresses: ");
    addr_list = (struct in_addr **) he->h_addr_list;
    for (i = 0; addr_list[i] != NULL; i++) {
        printf("%s ", inet_ntoa(*addr_list[i]));
    }
    printf("\n");

    // get the host name of 66.94.230.32:

    inet_aton(ipaddr, &addr);
    he = gethostbyaddr(&addr, sizeof(addr), AF_INET);
    if (he != NULL)
        printf("Host name: %s\n", he->h_name);
    else
        printf("Cannot find host %s by name \n", ipaddr);

    return 0;
}