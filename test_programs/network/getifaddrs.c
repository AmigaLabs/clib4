#include <arpa/inet.h>
#include <net/if.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <ifaddrs.h>
#include <stdio.h>
#include <string.h>
#include <proto/bsdsocket.h>

static void
print_addr(const char *label, const struct sockaddr *sa) {
    char buf[INET6_ADDRSTRLEN];
    if (!sa) {
        printf("  %-12s (none)\n", label);
        return;
    }
    if (sa->sa_family == AF_INET) {
        inet_ntop(AF_INET, &((struct sockaddr_in *)sa)->sin_addr, buf, sizeof buf);
        printf("  %-12s %s\n", label, buf);
    } else if (sa->sa_family == AF_INET6) {
        inet_ntop(AF_INET6, &((struct sockaddr_in6 *)sa)->sin6_addr, buf, sizeof buf);
        printf("  %-12s %s\n", label, buf);
    } else {
        printf("  %-12s family=%d\n", label, (int)sa->sa_family);
    }
}

static void
print_flags(unsigned int flags) {
    printf("  flags      ");
    if (flags & IFF_UP)        printf(" UP");
    if (flags & IFF_BROADCAST) printf(" BROADCAST");
    if (flags & IFF_LOOPBACK)  printf(" LOOPBACK");
    if (flags & IFF_RUNNING)   printf(" RUNNING");
    if (flags & IFF_MULTICAST) printf(" MULTICAST");
    if (flags & IFF_DEBUG)     printf(" DEBUG");
    printf(" (0x%x)\n", flags);
}

int main(void) {
    struct ifaddrs *ifap, *ifa;
    int ret;

    ret = getifaddrs(&ifap);
    if (ret != 0) {
        perror("getifaddrs");
        return 1;
    }

    for (ifa = ifap; ifa; ifa = ifa->ifa_next) {
        if (!ifa->ifa_addr)
            continue;

        printf("Interface: %s\n", ifa->ifa_name);
        print_flags(ifa->ifa_flags);
        print_addr("address:",   ifa->ifa_addr);
        print_addr("netmask:",   ifa->ifa_netmask);

        if (ifa->ifa_flags & IFF_BROADCAST)
            print_addr("broadcast:", ifa->ifa_broadaddr);
        else if (ifa->ifa_flags & IFF_POINTOPOINT)
            print_addr("dest:",      ifa->ifa_dstaddr);

        if (ifa->ifa_data) {
            const struct ifa_data *d = (const struct ifa_data *) ifa->ifa_data;
            printf("  mtu:         %u\n", d->ifa_mtu);
            printf("  metric:      %u\n", d->ifa_metric);
            printf("  rx packets:  %u\n", d->ifa_packets_in);
            printf("  tx packets:  %u\n", d->ifa_packets_out);
            printf("  errors:      %u\n", d->ifa_errors);
            printf("  overruns:    %u\n", d->ifa_overruns);
        }

        printf("\n");
    }

    freeifaddrs(ifap);
    return 0;
}
