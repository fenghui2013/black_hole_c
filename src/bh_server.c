#include <stdlib.h>
#include <stdio.h>

#include "bh_module.h"
#include "bh_server.h"

typedef struct bh_client bh_client;
struct bh_client {
    int sock_fd;  // client socket fd
    char *ip;
    int port;
    bh_buffer *recv_buffer;
    bh_buffer *send_buffer;
    bh_client *prev;
    bh_client *next;
};

typedef struct bh_clients bh_clients;
struct bh_clients {
    bh_client *first;
    bh_client *last;
    int clients_count;
};

struct bh_server {
    int sock_fd;
    char *ip;
    int port;
    bh_clients *clients;
};

bh_server *
bh_server_create() {
    bh_server *server = (bh_server *)malloc(sizeof(bh_server));
    bh_clients *clients = (bh_clients *)malloc(sizeof(bh_clients));
    clients->first = NULL;
    clients->last = NULL;
    clients->clients_count = 0;
    server->clients = clients;
    server->sock_fd = 0;
    server->ip = "";
    server->port = 0;

    return server;
}

void
bh_server_release(bh_server *server) {
    free(server->clients);
    if (server->sock_fd != 0) {
        bh_socket_close(server->sock_fd);
    }
    free(server);
    server = NULL;
}

void
bh_server_listen(bh_event *event, bh_server *server, char *ip, int port) {
    server->sock_fd = bh_socket_create();
    server->ip = ip;
    server->port = port;
    printf("sock_fd: %d\n", server->sock_fd);
    printf("ip: %s\n", server->ip);
    printf("port: %d\n", server->port);
    bh_socket_nonblocking(server->sock_fd);
    bh_socket_bind(server->sock_fd, server->ip, server->port);
    bh_socket_listen(server->sock_fd, 1024);
    bh_event_add(event, server->sock_fd);
}

void
bh_server_client_accept(bh_event *event, bh_server *server) {
    bh_client *client = (bh_client *)malloc(sizeof(bh_client));

    client->sock_fd = bh_socket_accept(server->sock_fd, &client->ip, &client->port);
    printf("new client ip: %s, port: %d, fd:%d\n", client->ip, client->port, client->sock_fd);
    bh_socket_nonblocking(client->sock_fd);
    client->recv_buffer = bh_buffer_create(1024, 8*1024);
    client->send_buffer = bh_buffer_create(1024, 8*1024);
    client->next = NULL;
    client->prev = NULL;
    if (server->clients->first == NULL) {
        server->clients->first = client;
        server->clients->last = client;
    } else {
        server->clients->last->next = client;
        client->prev = server->clients->last;
        server->clients->last = client;
    }
    server->clients->clients_count += 1;
    bh_module_init(client->sock_fd);
    bh_event_add(event, client->sock_fd);
}

int
bh_server_client_connect(bh_event *event, bh_server *server, char *ip, int port) {
    bh_client *client = (bh_client *)malloc(sizeof(bh_client));

    client->sock_fd = bh_socket_create();
    client->ip = ip;
    client->port = port;
    bh_socket_connect(client->sock_fd, client->ip, client->port);
    bh_socket_nonblocking(client->sock_fd);
    client->recv_buffer = bh_buffer_create(1024, 8*1024);
    client->send_buffer = bh_buffer_create(1024, 8*1024);
    client->next = NULL;
    client->prev = NULL;
    if (server->clients->first == NULL) {
        server->clients->first = client;
        server->clients->last = client;
    } else {
        server->clients->last->next = client;
        client->prev = server->clients->last;
        server->clients->last = client;
    }
    server->clients->clients_count += 1;
    bh_module_init(client->sock_fd);
    bh_event_add(event, client->sock_fd);

    return client->sock_fd;
}

static bh_client *
_find(bh_server *server, int sock_fd) {
    bh_client *client = server->clients->first;

    while (client) {
        if (client->sock_fd == sock_fd) break;
        client = client->next;
    }

    return client;
}

void
bh_server_client_close(bh_event *event, bh_server *server, int sock_fd) {
    bh_client *client = _find(server, sock_fd);
    if (client == NULL) return;
    printf("close client ip: %s, port: %d\n", client->ip, client->port);

    if (client->prev==NULL && client->next==NULL) {
        server->clients->first = NULL;
        server->clients->last = NULL;
    } else if (client->prev==NULL && client->next!=NULL) {
        server->clients->first = client->next;
        client->next->prev = NULL;
    } else if (client->prev!=NULL && client->next==NULL) {
        server->clients->last = client->prev;
        client->prev->next = NULL;
    } else {
        client->prev->next = client->next;
        client->next->prev = client->prev;
    }
    server->clients->clients_count -= 1;
    bh_event_del(event, sock_fd);
    bh_buffer_release(client->recv_buffer);
    bh_buffer_release(client->send_buffer);
    bh_socket_close(client->sock_fd);
    free(client);
    client = NULL;
}

/*
 * 1, socket normal
 * 0, socket close
 * -1, socket error
 */
int
bh_server_read(bh_server *server, int sock_fd) {
    bh_client *client = _find(server, sock_fd);
    char *buffer = NULL;
    int res, size;

    if (client == NULL) return -1;
    while (1) {
        size = bh_buffer_get_write(client->recv_buffer, &buffer);
        res = bh_socket_recv(sock_fd, &buffer, size);
        if (res == -2) break;     // again
        if (res == -1) return -1; // error close
        if (res == 0) return 0;   // normal close
        bh_buffer_set_write(client->recv_buffer, res);
    }
    return 1;
}

/*
 * 0, data wirte done
 * 1, again
 * -1, error
 */
int
bh_server_write(bh_server *server, int sock_fd) {
    bh_client *client = _find(server, sock_fd);
    char *buffer = NULL;
    int res, size;

    if (client == NULL) return -1;
    while (1) {
        size = bh_buffer_get_read(client->send_buffer, &buffer);
        if (size == 0) return 0;
        res = bh_socket_send(sock_fd, buffer, size);
        if (res == -2) break;
        if (res == -1) return -1;
        bh_buffer_set_read(client->send_buffer, res);
        //switch(res) {
        //    case -2:
        //        // again
        //        break;
        //    case -1:
        //        // error
        //        return -1;
        //    default:
        //        bh_buffer_set_read(client->send_buffer, res);
        //}
    }
    return 1;
}

void
up_to_down(bh_event *event, bh_server *server, int sock_fd, char *data, int len) {
    bh_client *client = _find(server, sock_fd);
    char *buffer = NULL;
    int size, i, j = 0;

    if (client == NULL) return;
    while (1) {
        size = bh_buffer_get_write(client->send_buffer, &buffer);
        for (i=0; i<size && j<len; i++, j++) {
            buffer[i] = data[j];
        }
        bh_buffer_set_write(client->send_buffer, i);
        if (j == len) break;
    }
    bh_event_write(event, sock_fd, 1);
}

void
down_to_up(bh_server *server, int sock_fd) {
    bh_client *client = _find(server, sock_fd);
    char *buffer = NULL;
    int size;

    if (client == NULL) return;
    while (1) {
        size = bh_buffer_get_read(client->recv_buffer, &buffer);
        if (size == 0) break;
        bh_module_recv(sock_fd, buffer, size);
        bh_buffer_set_read(client->recv_buffer, size);
    }
}

void
bh_server_run(bh_event *event, bh_server *server, bh_timer *timer) {
    int events_num = 0, i, res, timeout;
    
    while (1) {
        timeout = bh_timer_get(timer);
        events_num = bh_event_poll(event, event->max_events, timeout);
        for (i=0; i<events_num; i++) {
            if (event->events[i].fd == server->sock_fd) {
                // accept new client
                bh_server_client_accept(event, server);
            } else {
                if (event->events[i].read) {
                    res = bh_server_read(server, event->events[i].fd);
                    if (res == 1) {
                        down_to_up(server, event->events[i].fd);
                    } else if (res == 0 || res == -1) {
                        bh_module_recv(event->events[i].fd, "", 0);
                        bh_server_client_close(event, server, event->events[i].fd);
                    }
                }

                if (event->events[i].write) {
                    res = bh_server_write(server, event->events[i].fd);
                    // write data done
                    if (res == 0) {
                        bh_event_write(event, event->events[i].fd, 0);
                    }
                    // write data error
                    if (res == -1) {
                        bh_server_client_close(event, server, event->events[i].fd);
                    }
                }
            }
        }
        bh_timer_execute(timer);
    }
}
