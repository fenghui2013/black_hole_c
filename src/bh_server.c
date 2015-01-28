#include <stdlib.h>

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
bh_server_listen(bh_event *event, bh_server *server, char *ip, int port) {
    server->sock_fd = bh_socket_create();
    server->ip = ip;
    server->port = port;
    bh_socket_bind(server->sock_fd, server->ip, server->port);
    bh_socket_nonblocking(server->sock_fd);
    bh_socket_listen(server->sock_fd, 1024);
    bh_event_add(event->event_fd, server->sock_fd);
}

void
bh_server_client_accept(bh_event *event, bh_server *server) {
    bh_client *client = (bh_client *)malloc(sizeof(bh_client));

    client->sock_fd = bh_socket_accept(server->sock_fd, &client->ip, &client->port);
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
    bh_event_add(event->event_fd, client->sock_fd);
}

void
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
    bh_event_add(event->event_fd, client->sock_fd);
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
    server->clients->client_count -= 1;
    bh_event_del(event->event_fd, sock_fd);
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
    char *buffer;
    int res, size;

    while (1) {
        size = bh_buffer_get_write(client->recv_buffer, buffer);
        res = bh_socket_recv(sock_fd, &buffer, size);
        switch (res) {
            case -2:
                // again
                break;
            case -1:
                // error close
                return -1;
            case 0:
                // normal close
                return 0;
            default:
                bh_buffer_set_write(client->recv_buffer, res);
        }
    }
    return 1;
}

/*
 * 1, normal
 * -1, error
 */
int
bh_server_write(bh_server *server, int sock_fd) {
    bh_client *client = _find(server, sock_fd);
    char *buffer;
    int res, size;

    while (1) {
        size = bh_buffer_get_read(client->send_buffer, buffer);
        if (size == 0) return 0;
        res = bh_socket_send(sock_fd, buffer);
        switch(res) {
            case -2:
                // again
                break;
            case -1:
                // error
                return -1;
            default:
                bh_buffer_set_read(client->send_buffer, res);
        }
    }
    return 1;
}

void
send_to_application_layer(bh_server *server, int sock_fd) {
    bh_client *client = _find(server, sock_fd);
    char *buffer;
    int size;

    while (1) {
        size = bh_buffer_get_read(client->send_buffer, buffer);
        if (size == 0) break;
        bh_module_recv(sock_fd, buffer);
        bh_buffer_set_read(client->recv_buffer, size);
    }
}

void
bh_server_run(bh_event *event, bh_server *server, bh_timer *timer) {
    int events_num = 0, i, res, timeout;
    
    while (1) {
        timeout = bh_timer_get(timer);
        events_num = bh_event_poll(event->event_fd, event->events, event->max_events, timeout);
        for (i=0; i<events_num; i++) {
            f (event->events[i].fd == server->sock_fd) {
                // accept new client
                bh_server_accept_client(event, server);
            } else {
                if (event->events[i].read) {
                    res = bh_server_read(server, event->events[i].fd);
                    if (res == 1) {
                        send_to_application_layer(server, event->events[i].fd);
                    } else if (res == 0 || res == -1) {
                        bh_server_close_client(server, event->events[i].fd);
                        bh_module_recv(event->events[i].fd, "");
                    }
                }

                if (event->events[i].write) {
                    res = bh_server_write(server, event->events[i].fd);
                    // write data done
                    if (res == 0) {
                        bh_event_del(event, event->events[i].fd);
                    }
                    // write data error
                    if (res == -1) {
                        bh_server_close_client(server, event->events[i].fd);
                    }
                }
            }
        }
        bh_timer_execute(timer);
    }
}
