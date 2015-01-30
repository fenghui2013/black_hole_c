#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>

#include "bh_socket.h"

int
bh_socket_create() {
    int sock_fd;

    sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == sock_fd) {
       return -1; 
    }
    return sock_fd;
}

int
bh_socket_close(int sock_fd) {
    int res;

    res = close(sock_fd);
    if (-1 == res) {
        return -1;
    }
    return res;
}

void
bh_socket_nonblocking(int sock_fd) {
    int flag;

    flag = fcntl(sock_fd, F_GETFL, 0);
    if (-1 == flag) {
        return;
    }
    fcntl(sock_fd, F_SETFL, flag | O_NONBLOCK);
}

void
bh_socket_tcpnodelay(int sock_fd, int enable) {
    int res;

    res = setsockopt(sock_fd, IPPROTO_TCP, TCP_NODELAY, &enable, sizeof(enable));
    if (-1 == res) {
        return;
    }
}

int
bh_socket_bind(int sock_fd, const char *ip, int port) {
    int res;
    struct sockaddr_in server_addr;

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    inet_aton(ip, &server_addr.sin_addr);
    server_addr.sin_port = htons(port);

    res = bind(sock_fd, (const struct sockaddr *)&server_addr, sizeof(struct sockaddr));
    if (-1 == res) {
        return -1;
    }
    return res;
}

int
bh_socket_listen(int sock_fd, int backlog) {
    int res;
    
    res = listen(sock_fd, backlog);
    if (-1 == res) {
        return -1;
    }
    return res;
}

/*
 * new_fd > 0, normal
 * new_fd == 0, again
 * new_fd == -1, error
 */
int
bh_socket_accept(int sock_fd, char **ip, int *port) {
    int new_fd;
    struct sockaddr_in remote_addr;
    socklen_t len = sizeof(remote_addr);
    memset(&remote_addr, 0, sizeof(remote_addr));
    
    new_fd = accept(sock_fd, (struct sockaddr *)&remote_addr, &len);
    if (-1 == new_fd) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return 0;
        }
        return -1;
    }

    *ip = inet_ntoa(remote_addr.sin_addr);
    *port = ntohs(remote_addr.sin_port);

    return new_fd;
}

int
bh_socket_connect(int sock_fd, const char *ip, int port) {
    int res;
    struct sockaddr_in new_addr;

    memset(&new_addr, 0, sizeof(new_addr));
    new_addr.sin_family = AF_INET;
    inet_aton(ip, &new_addr.sin_addr);
    new_addr.sin_port = htons(port);

    res = connect(sock_fd, (const struct sockaddr *)&new_addr, sizeof(struct sockaddr));
    if (-1 == res) {
        return -1;
    }
    return res;
}
/*
 * n >= 0, normal
 * n == -1, error
 * n == -2, again
 */
int
bh_socket_recv(int sock_fd, char **buffer, int size) {
    int n;

    n = read(sock_fd, *buffer, size);
    if (-1 == n) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return -2;
        }
        printf("%s\n", strerror(errno));
        return -1;
    }
    return n;
}

/*
 * n >= 0, normal
 * n == -1, error
 * n == -2, again
 */
int
bh_socket_send(int sock_fd, const char *buffer, int size) {
    int n;

    n = write(sock_fd, buffer, size);
    if (-1 == n) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return -2;
        }
        return -1;
    }
    return n;
}
