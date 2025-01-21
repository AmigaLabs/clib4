/* inet_network.c:
 * example illustrates how the inet_network(3) function can be used.
 * The program also calls upon the htonl function to display how the
 * value looks in network endian order
 * Example using inet_network(3):
 */
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char **argv) {
    int x;
    const char *addr[] = {
            "44.135.86.12",
            "127.0.0.1",
            "172.16.23.95",
            "192.168.9.1"
    };
    unsigned int net_addr;

    for (x = 0; x < 4; ++x) {
        net_addr = inet_network(addr[x]);
        printf("%14s = 0x%05X net 0x%05X\n", addr[x], net_addr,
               (unsigned int) htonl(net_addr));
    }

    return 0;
}