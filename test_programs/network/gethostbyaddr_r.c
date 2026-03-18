#include <netdb.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
const char *ip_str = "8.8.8.8";
    struct in_addr addr;

    if (inet_pton(AF_INET, ip_str, &addr) != 1) {
        perror("inet_pton failed");
        return 1;
    }

    struct hostent result_buf;
    struct hostent *result = NULL;

    char buffer[8192];
    int h_err;

    int ret = gethostbyaddr_r(
        &addr,
        sizeof(addr),
        AF_INET,
        &result_buf,
        buffer,
        sizeof(buffer),
        &result,
        &h_err
    );

    if (ret != 0 || result == NULL) {
        fprintf(stderr, "Lookup failed: %s (h_errno=%d)\n",
                hstrerror(h_err), h_err);
        return 1;
    }

    printf("Hostname: %s\n", result->h_name);

    // Alias
    char **alias = result->h_aliases;
    while (*alias) {
        printf("Alias: %s\n", *alias++);
    }

    // Indirizzi associati
    char ipbuf[INET_ADDRSTRLEN];
    char **addr_list = result->h_addr_list;
    while (*addr_list) {
        inet_ntop(AF_INET, *addr_list, ipbuf, sizeof(ipbuf));
        printf("Address: %s\n", ipbuf);
        addr_list++;
    }

    return 0;
}