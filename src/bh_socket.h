#ifndef _BH_SOCKET_H_
#define _BH_SOCKET_H_

int  bh_socket_create();
int  bh_socket_close(int sock_fd);
void bh_socket_nonblocking(int sock_fd);
void bh_socket_tcpnodelay(int sock_fd, int enable);
int  bh_socket_bind(int sock_fd, const char *ip, int port);
int  bh_socket_listen(int sock_fd, int backlog);
int  bh_socket_accept(int sock_fd, char **ip, int *port);

int  bh_socket_connect(int sock_fd, const char *ip, int port);

int  bh_socket_recv(int sock_fd, char **buffer, int size);
int  bh_socket_send(int sock_fd, const char *buffer, int size);
#endif
