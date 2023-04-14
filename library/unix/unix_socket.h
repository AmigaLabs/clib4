#ifndef _UNIX_SOCKET_H
#define _UNIX_SOCKET_H

int get_socket_type(int sockfd);
int get_addr_family(int sockfd);
int get_addr_protocol(int sockfd);
void af_local_set_secret(int sockfd, char *buf);
void af_local_set_cred(int sockfd);
int get_inet_addr(const struct sockaddr *in, int inlen, struct sockaddr_storage *out, socklen_t *outlen, int *type, int *secret);
void set_peer_sun_path(int sockfd, const char *path);
const char *get_peer_sun_path(int sockfd);

#endif