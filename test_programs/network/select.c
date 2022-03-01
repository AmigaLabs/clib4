#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

/* port we're listening on - Use telnet IPADDRESS 2020 to connect */
#define PORT 2020

int main(int argc, char *argv[]) {
    /* master file descriptor list */
    fd_set master;
    /* temp file descriptor list for select() */
    fd_set read_fds;
    /* server address */
    struct sockaddr_in serveraddr;
    /* client address */
    struct sockaddr_in clientaddr;
    /* maximum file descriptor number */
    int fdmax;
    /* listening socket descriptor */
    int listener;
    /* newly accept()ed socket descriptor */
    int newfd;
    /* buffer for client data */
    char buf[1024];
    int nbytes;

    /* for setsockopt() SO_REUSEADDR, below */
    int yes = 1;
    socklen_t addrlen;
    int i, j;

    /* clear the master and temp sets */
    FD_ZERO(&master);
    FD_ZERO(&read_fds);

    /* get the listener */
    if ((listener = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Server-socket() error!");
        exit(1);
    }

    printf("Server-socket() is OK...\n");
    /*"address already in use" error message */
    if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
        perror("Server-setsockopt() error!");
        exit(1);
    }
    printf("Server-setsockopt() is OK...\n");
    /* bind */
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = INADDR_ANY;
    serveraddr.sin_port = htons(PORT);
    memset(&(serveraddr.sin_zero), '\0', 8);

    if (bind(listener, (struct sockaddr *) &serveraddr, sizeof(serveraddr)) == -1) {
        perror("Server-bind() error!");
        exit(1);
    }
    printf("Server-bind() is OK...\n");
    /* listen */
    if (listen(listener, 10) == -1) {
        perror("Server-listen() error!");
        exit(1);
    }
    printf("Server-listen() is OK...\n");
    /* add the listener to the master set */
    FD_SET(listener, &master);
    /* keep track of the biggest file descriptor */
    fdmax = listener; /* so far, it's this one*/
    /* loop */
    for (;;) {
        /* copy it */
        read_fds = master;
        if (select(fdmax + 1, &read_fds, NULL, NULL, NULL) == -1) {
            perror("Server-select() error!");
            exit(1);
        }
        printf("Server-select() is OK...\n");
        /*run through the existing connections looking for data to be read*/
        for (i = 0; i <= fdmax; i++) {
            if (FD_ISSET(i, &read_fds)) { /* we got one... */
                if (i == listener) {
                    /* handle new connections */
                    addrlen = sizeof(clientaddr);
                    if ((newfd = accept(listener, (struct sockaddr *) &clientaddr, &addrlen)) == -1) {
                        perror("Server-accept() error!");
                    } else {
                        printf("Server-accept() is OK...\n");
                        FD_SET(newfd, &master); /* add to master set */
                        if (newfd > fdmax) { /* keep track of the maximum */
                            fdmax = newfd;
                        }
                        printf("%s: New connection from %s on socket %d\n", argv[0], inet_ntoa(clientaddr.sin_addr),
                               newfd);
                    }
                } else {
                    /* handle data from a client */
                    if ((nbytes = recv(i, buf, sizeof(buf), 0)) <= 0) {
                        /* got error or connection closed by client */
                        if (nbytes == 0)
                            /* connection closed */
                            printf("%s: socket %d hung up\n", argv[0], i);
                        else
                            perror("recv() error!");
                        /* close it... */
                        close(i);
                        /* remove from master set */
                        FD_CLR(i, &master);
                    } else {
                        /* we got some data from a client*/
                        for (j = 0; j <= fdmax; j++) {
                            /* send to everyone! */
                            if (FD_ISSET(j, &master)) {
                                /* except the listener and ourselves */
                                if (j != listener && j != i) {
                                    if (send(j, buf, nbytes, 0) == -1)
                                        perror("send() error!");
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    return 0;
}