#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>

#define PORT 9990
#define SIZE 1024

int main(int argc, char *argv[]) {

    if (argc != 3) {
        printf("Usage: %s <host> <port>\n", argv[0]);
        return -1;
    }

    char *hostArgv = argv[1];
    char *portArgv = argv[2];

    int port = atoi(portArgv);

    int client_socket = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    inet_aton(hostArgv, &(addr.sin_addr));

    int server_socket = connect(client_socket, (struct sockaddr *) &addr, sizeof(addr)); //连接服务器

    printf("Connect result ==> %d client_socket = %d\n", server_socket, client_socket);
    if (server_socket < 0) {
        perror("Connect result");
        exit(1);
    }

    char buf[SIZE] = {0};

    while (1) {
        printf("Send to server: ");
        scanf("%s", buf);
        ssize_t written = write(client_socket, buf, strlen(buf));
        printf("written %ld bytes\n", written);
        if (strncmp(buf, "end", 3) == 0) {
            break;
        }
    }
    close(server_socket);

    return 0;
}