// Simple example of server with select() and multiple clients.

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <unistd.h>

#include "message.h"

#define MAX_CLIENTS 10

#define NO_SOCKET -1

#define SERVER_NAME "server"

int listen_sock;
peer_t connection_list[MAX_CLIENTS];
char read_buffer[1024]; // buffer for stdin

void shutdown_properly(int code);

void handle_signal_action(int sig_number) {
    if (sig_number == SIGINT) {
        printf("SIGINT was catched!\n");
        shutdown_properly(EXIT_SUCCESS);
    } else if (sig_number == SIGPIPE) {
        printf("SIGPIPE was catched!\n");
        shutdown_properly(EXIT_SUCCESS);
    }
}

int setup_signals() {
    struct sigaction sa;
    sa.sa_handler = handle_signal_action;
    if (sigaction(SIGINT, &sa, 0) != 0) {
        perror("sigaction()");
        return -1;
    }
    if (sigaction(SIGPIPE, &sa, 0) != 0) {
        perror("sigaction()");
        return -1;
    }

    return 0;
}

/* Start listening socket listen_sock. */
int start_listen_socket(int *listen_sock) {
    // Obtain a file descriptor for our "listening" socket.
    *listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (*listen_sock < 0) {
        perror("socket");
        return -1;
    }

    int reuse = 1;
    if (setsockopt(*listen_sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) != 0) {
        perror("setsockopt");
        return -1;
    }

    struct sockaddr_in my_addr;
    memset(&my_addr, 0, sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_addr.s_addr = inet_addr(SERVER_IPV4_ADDR);
    my_addr.sin_port = htons(SERVER_LISTEN_PORT);

    if (bind(*listen_sock, (struct sockaddr *) &my_addr, sizeof(struct sockaddr)) != 0) {
        perror("bind");
        return -1;
    }

    // start accept client connections
    if (listen(*listen_sock, 10) != 0) {
        perror("listen");
        return -1;
    }
    printf("Accepting connections on port %d.\n", (int) SERVER_LISTEN_PORT);

    return 0;
}

void shutdown_properly(int code) {
    int i;

    close(listen_sock);

    for (i = 0; i < MAX_CLIENTS; ++i)
        if (connection_list[i].socket != NO_SOCKET)
            close(connection_list[i].socket);

    printf("Shutdown server properly.\n");
    exit(code);
}

int build_fd_sets(fd_set *read_fds, fd_set *write_fds, fd_set *except_fds) {
    int i;

    FD_ZERO(read_fds);
    FD_SET(STDIN_FILENO, read_fds);
    FD_SET(listen_sock, read_fds);
    for (i = 0; i < MAX_CLIENTS; ++i)
        if (connection_list[i].socket != NO_SOCKET)
            FD_SET(connection_list[i].socket, read_fds);

    FD_ZERO(write_fds);
    for (i = 0; i < MAX_CLIENTS; ++i)
        if (connection_list[i].socket != NO_SOCKET && connection_list[i].send_buffer.current > 0)
            FD_SET(connection_list[i].socket, write_fds);

    FD_ZERO(except_fds);
    FD_SET(STDIN_FILENO, except_fds);
    FD_SET(listen_sock, except_fds);
    for (i = 0; i < MAX_CLIENTS; ++i)
        if (connection_list[i].socket != NO_SOCKET)
            FD_SET(connection_list[i].socket, except_fds);

    return 0;
}

int handle_new_connection() {
    struct sockaddr_in client_addr;
    memset(&client_addr, 0, sizeof(client_addr));
    socklen_t client_len = sizeof(client_addr);
    int new_client_sock = accept(listen_sock, (struct sockaddr *) &client_addr, &client_len);
    if (new_client_sock < 0) {
        perror("accept()");
        return -1;
    }

    char client_ipv4_str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr.sin_addr, client_ipv4_str, INET_ADDRSTRLEN);

    printf("Incoming connection from %s:%d.\n", client_ipv4_str, client_addr.sin_port);

    int i;
    for (i = 0; i < MAX_CLIENTS; ++i) {
        if (connection_list[i].socket == NO_SOCKET) {
            connection_list[i].socket = new_client_sock;
            connection_list[i].addres = client_addr;
            connection_list[i].current_sending_byte = -1;
            connection_list[i].current_receiving_byte = 0;
            return 0;
        }
    }

    printf("There is too much connections. Close new connection %s:%d.\n", client_ipv4_str, client_addr.sin_port);
    close(new_client_sock);
    return -1;
}

int close_client_connection(peer_t *client) {
    printf("Close client socket for %s.\n", peer_get_addres_str(client));

    close(client->socket);
    client->socket = NO_SOCKET;
    dequeue_all(&client->send_buffer);
    client->current_sending_byte = -1;
    client->current_receiving_byte = 0;
}

/* Reads from stdin and create new message. This message enqueues to send queueu. */
int handle_read_from_stdin() {
    char read_buffer[DATA_MAXSIZE]; // buffer for stdin
    if (read_from_stdin(read_buffer, DATA_MAXSIZE) != 0) {
        return -1;
    }

    // Create new message and enqueue it.
    message_t new_message;
    prepare_message(SERVER_NAME, read_buffer, &new_message);
    print_message(&new_message);

    /* enqueue message for all clients */
    int i;
    for (i = 0; i < MAX_CLIENTS; ++i) {
        if (connection_list[i].socket != NO_SOCKET) {
            if (peer_add_to_send(&connection_list[i], &new_message) != 0) {
                printf("Send buffer was overflowed, we lost this message!\n");
                continue;
            }
            printf("New message to send was enqueued right now.\n");
        }
    }

    return 0;
}

int handle_received_message(message_t *message) {
    printf("Received message from client.\n");
    print_message(message);
    return 0;
}

int main(int argc, char **argv) {
    if (setup_signals() != 0)
        exit(EXIT_FAILURE);

    if (start_listen_socket(&listen_sock) != 0)
        exit(EXIT_FAILURE);

    /* Set nonblock for stdin. */
    int flag = fcntl(STDIN_FILENO, F_GETFL, 0);
    flag |= O_NONBLOCK;
    fcntl(STDIN_FILENO, F_SETFL, flag);

    int i;
    for (i = 0; i < MAX_CLIENTS; ++i) {
        connection_list[i].socket = NO_SOCKET;
        create_peer(&connection_list[i]);
    }

    fd_set read_fds;
    fd_set write_fds;
    fd_set except_fds;

    int high_sock = listen_sock;

    printf("Waiting for incoming connections.\n");

    while (1) {
        build_fd_sets(&read_fds, &write_fds, &except_fds);

        high_sock = listen_sock;
        for (i = 0; i < MAX_CLIENTS; ++i) {
            if (connection_list[i].socket > high_sock)
                high_sock = connection_list[i].socket;
        }

        int activity = select(high_sock + 1, &read_fds, &write_fds, &except_fds, NULL);

        switch (activity) {
            case -1:
                perror("select()");
                shutdown_properly(EXIT_FAILURE);

            case 0:
                // you should never get here
                printf("select() returns 0.\n");
                shutdown_properly(EXIT_FAILURE);

            default:
                /* All set fds should be checked. */
                if (FD_ISSET(STDIN_FILENO, &read_fds)) {
                    if (handle_read_from_stdin() != 0)
                        shutdown_properly(EXIT_FAILURE);
                }

                if (FD_ISSET(listen_sock, &read_fds)) {
                    handle_new_connection();
                }

                if (FD_ISSET(STDIN_FILENO, &except_fds)) {
                    printf("except_fds for stdin.\n");
                    shutdown_properly(EXIT_FAILURE);
                }

                if (FD_ISSET(listen_sock, &except_fds)) {
                    printf("Exception listen socket fd.\n");
                    shutdown_properly(EXIT_FAILURE);
                }

                for (i = 0; i < MAX_CLIENTS; ++i) {
                    if (connection_list[i].socket != NO_SOCKET && FD_ISSET(connection_list[i].socket, &read_fds)) {
                        if (receive_from_peer(&connection_list[i], &handle_received_message) != 0) {
                            close_client_connection(&connection_list[i]);
                            continue;
                        }
                    }

                    if (connection_list[i].socket != NO_SOCKET && FD_ISSET(connection_list[i].socket, &write_fds)) {
                        if (send_to_peer(&connection_list[i]) != 0) {
                            close_client_connection(&connection_list[i]);
                            continue;
                        }
                    }

                    if (connection_list[i].socket != NO_SOCKET && FD_ISSET(connection_list[i].socket, &except_fds)) {
                        printf("Exception client fd.\n");
                        close_client_connection(&connection_list[i]);
                        continue;
                    }
                }
        }

        printf("And we are still waiting for clients' or stdin activity. You can type something to send:\n");
    }

    return 0;
}