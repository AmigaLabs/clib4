#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>

#define MAXFD 10

int create_sockfd();

//Add the file descriptor fd to the fds array to specify events of interest
void fds_add(struct pollfd fds[], int fd) {
    int i = 0;
    for (; i < MAXFD; ++i) {
        if (fds[i].fd == -1) {
            fds[i].fd = fd;
            fds[i].events = POLLIN;
            fds[i].revents = 0;
            break;
        }
    }

}

//Deletes the specified file descriptor fd and its associated event information from the fds array
void fds_del(struct pollfd fds[], int fd) {
    int i = 0;
    for (; i < MAXFD; ++i) {
        if (fds[i].fd == fd) {
            fds[i].fd = -1;
            fds[i].events = 0;
            fds[i].revents = 0;
            break;
        }
    }
}

//Initialize the fds array with the file descriptor fd set to -1 and the rest set to 0
void fds_init(struct pollfd fds[]) {
    int i = 0;
    for (; i < MAXFD; ++i) {
        fds[i].fd = -1;
        fds[i].events = 0;
        fds[i].revents = 0;
    }
}

int main() {
    int sockfd = create_sockfd();
    assert(sockfd != -1);

    //Initialize pollfd type struct array fds
    struct pollfd fds[MAXFD];

    //Initialize fds array
    fds_init(fds);

    //Add sockfd to fds array
    fds_add(fds, sockfd);

    while (1) {
        //Use poll system call polling to test for readiness
        int n = poll(fds, MAXFD, 5000);
        if (n == -1) {
            perror("poll error");
        } else if (n == 0)    //Return 0 indicates timeout, no ready
        {
            printf("time out\n");
        } else    //File descriptor ready for fds array
        {
            int i = 0;
            for (; i < MAXFD; ++i) {
                if (fds[i].fd == -1) {
                    continue;
                }

                //Which file descriptor is available and the member revents in fds[i] are modified by the system kernel
                if (fds[i].revents & POLLIN) {
                    //There are two situations at this time.
                    //If fds[i].fd == sockfd indicates that there are connections pending processing in the listening queue
                    //Then use accept to receive the connection
                    //Otherwise, no new connection will be made and data will be sent from an existing client.
                    //We use recv directly to receive client data and print the data we receive
                    if (fds[i].fd == sockfd) {
                        struct sockaddr_in caddr;
                        long len = sizeof(caddr);

                        //Receive a socket established connection to get the connection socket c
                        int c = accept(sockfd, (struct sockaddr *) &caddr, &len);
                        if (c < 0) {
                            continue;
                        }
                        printf("accept c=%d\n", c);
                        //Add a new connection socket c to the array fds of pollfd structure type
                        fds_add(fds, c);//join
                    } else {
                        char buff[128] = {0};

                        //Receive client data using recv
                        int res = recv(fds[i].fd, buff, 127, 0);

                        //The data on the receiving server side is zero, that is, n returns 0, indicating that the client has been shut down
                        if (res <= 0) {
                            //Close file descriptor fds[i].fd
                            close(fds[i].fd);
                            //Remove this file descriptor from the fds array
                            fds_del(fds, fds[i].fd);
                            printf("one client over\n");
                        } else {
                            //n is not zero, that is, when data is received, print the data and reply to the client with a message
                            printf("recv(%d)=%s\n", fds[i].fd, buff);
                            send(fds[i].fd, "ok", 2, 0);
                        }
                    }
                }
            }
        }
    }
}

int create_sockfd() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        return -1;
    }

    struct sockaddr_in saddr;
    memset(&saddr, 0, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_port = htons(6000);
    saddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    int res = bind(sockfd, (struct sockaddr *) &saddr, sizeof(saddr));
    if (res == -1) {
        return -1;
    }

    listen(sockfd, 5);

    return sockfd;
}
