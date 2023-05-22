#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char **argv) {
    int sockfd, sendbuff;
    socklen_t optlen;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == -1)
        printf("Error");

    int res = 0;

    // Get buffer size
    optlen = sizeof(sendbuff);
    res = getsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, &sendbuff, &optlen);

    if (res == -1)
        printf("Error getsockopt one\n");
    else
        printf("send buffer size = %d\n", sendbuff);

    // Set buffer size
    sendbuff = 98304 * 2;

    printf("sets the send buffer to %d\n", sendbuff);
    res = setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, &sendbuff, sizeof(sendbuff));

    if (res == -1)
        printf("Error setsockopt\n");


    // Get buffer size
    optlen = sizeof(sendbuff);
    res = getsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, &sendbuff, &optlen);

    if (res == -1)
        printf("Error getsockopt two");
    else
        printf("send buffer size = %d\n", sendbuff);

    close(sockfd);

    return 0;
}