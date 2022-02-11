#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>

#define INADDR  "192.168.0.47"

int
main() {
    struct in_addr inaddr;
    char buf[INET_ADDRSTRLEN];
    int rval;

    if ((rval = inet_pton(AF_INET, INADDR, &inaddr)) == 0) {
        printf("Invalid address: %s\n", INADDR);
        exit(EXIT_FAILURE);
    } else if (rval == -1) {
        perror("inet_pton");
        exit(EXIT_FAILURE);
    }

    if (inet_ntop(AF_INET, &inaddr, buf, sizeof(buf)) != NULL)
        printf("inet addr: %s\n", buf);
    else {
        perror("inet_ntop");
        exit(EXIT_FAILURE);
    }

    return (EXIT_SUCCESS);
}
