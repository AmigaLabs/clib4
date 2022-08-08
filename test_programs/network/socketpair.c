#include <sys/param.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/wait.h>

#include <errno.h>
#include <pwd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <unistd.h>
#include <netdb.h>

int main(int argc, char *argv[])
{
    int ch, d = 0, pair[2];

    if (socketpair(AF_UNIX, SOCK_STREAM, AF_UNSPEC, pair) == -1) {
        printf("socketpair unix error %d %d\n", errno, h_errno);
        exit(1);
    }
    printf("Unix sokcketpair ok: %d - %d\n", pair[0], pair[1]);

    close(pair[0]);
    close(pair[1]);

    if (socketpair(AF_INET, SOCK_STREAM, AF_UNSPEC, pair) == -1) {
        printf("socketpair inet error\n");
        exit(1);
    }
    
    printf("Inet sokcketpair ok: %d - %d\n", pair[0], pair[1]);

    close(pair[0]);
    close(pair[1]);

    return (0);
}