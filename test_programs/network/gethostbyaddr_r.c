#include <netdb.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
    char *address = "8.8.8.8";
    int length = strlen(address);
    int type = AF_INET;
    struct hostent h;
    int rc = 0;

    char buffer[8192], **c;
    int h_errnop;
    struct hostent *hp;

    rc = gethostbyaddr_r(address, length, type, &h, buffer, 8192, &hp, &h_errnop);

    if (rc != 0)
        printf("rc = %d %s\n", rc, strerror(rc));
    else {
        printf("%s\n", h.h_name);
        for (c = h.h_addr_list; *c; c++) {
            printf(" %s\n", inet_ntop(h.h_addrtype, *c, buffer, sizeof buffer));
        }
    }

    return 0;
}