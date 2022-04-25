#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>

int
main(int argc, char **argv) {
    struct in_addr inaddr;
    struct in6_addr inaddr6;
    char buf[INET6_ADDRSTRLEN];
    int rval;
    int family;

    if (argc != 2)
    {
        printf("usage: inet_ntop <ip4/6 address>\n");
        return 0;
    }

    /* Check for IPV4 address */
    rval = inet_pton(AF_INET, argv[1], &inaddr);
    if (rval <= 0) {
        printf("%s is not an IPV4 address. Check for IPV6\n", argv[1]);
        /* Check for IPV6 address on error */
        rval = inet_pton(AF_INET6, argv[1], &inaddr);
        if (rval > 0)
            family = AF_INET6;
    }
    else
        family = AF_INET;

    if (rval == 0) {
        printf("Invalid address: %s\n", argv[1]);
        exit(EXIT_FAILURE);
    } else if (rval == -1) {
        perror("inet_pton");
        exit(EXIT_FAILURE);
    }

    const char *result;
    if (family == AF_INET)
        result = inet_ntop(family, &inaddr, buf, sizeof(buf));
    else
        result = inet_ntop(family, &inaddr6, buf, sizeof(buf));
    if (result != NULL)
        printf("inet addr (%s): %s\n", family == AF_INET ? "AF_INET" : "AF_INET6", buf);
    else {
        perror("inet_ntop");
        exit(EXIT_FAILURE);
    }

    return (EXIT_SUCCESS);
}
